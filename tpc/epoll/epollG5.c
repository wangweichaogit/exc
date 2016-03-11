/*
G5的连接转发demo
运行程序访问8080端口会转接到8090端口。
 192.168.119.250::8080  -> 192.168.119.250::8090
 
 负载均衡服务器：接受一个连接，自身新建一个连接，连接到 地址池中的一个地址上；保持信息的转载
 
 test步骤:
 1.执行转发程序 ./epollG5 
 2.nc 模拟server端  nc -l 192.168.119.250 8090
 3.nc 模拟client端  nc 192.168.119.250 8080
 
 
 EPOLLIN 和 EPOLLOUT
 epollout触发： 非阻塞send时候 写缓冲道具由满清空
 								非阻塞connect时  。。。
 EPOLLOUT在这里用在两个地方(异步回调)： 1 非阻塞connect连接目标时，没有立即连接上时 设置epoll_ctl监听fd上的epoll
 out事件，事件发生进行回调   （非阻塞connect + epollout回调 + getsockopt返回值判断connect是否连接上）
 也可以（非阻塞connect + select监听可写集合 + getsockopt返回值判断connect是否连接上）
 2. send数据返回失败  epoll_ctl对sockfd设置为epollout，通过epoll的回调函数发送数据，发送完数据后epoll_ctl再将socketfd
 设置为epollin读监听
*/
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define BUFSIZE 4096      //接受buff大小
#define EVENTSIZE  10000    //epoll事件集合大小

const char * testip = "127.0.0.1";
const char * testip2 = "127.0.0.1";
int  testport = 8080;
int testport2 = 8090;

//	节点地址 和 sock描述字
struct Node{
	struct sockaddr_in sockaddr;
	char ip[64];
	int port;
	int sock;
};

//程序的环境变量结构
struct ServerEnv{
	int 				epoll_fds;      //epoll描述字
	struct 			epoll_event events[EVENTSIZE];   //epoll 事件集合
	
	int         listenfd;       //  监听sockket
	struct Node listen_addr;   	//  待转发的地址结构
	struct Node server_addr;		//   转发的地址结构
	char   			buf[BUFSIZE];    //信息收发buf
	int					buflen;        //buf长度
	int 				sock_count;    //返回的epoll事件数目
};

// 设置地址重用
int SetReuseAddr( int sock )
{
	int	on ;
	
	on = 1 ;
	setsockopt( sock , SOL_SOCKET , SO_REUSEADDR , (void *) & on, sizeof(on) );
	
	return 0;
}

//套接字设置为非阻塞
int SetNonBlocking( int sock )
{
	int	opts;
	opts = fcntl( sock , F_GETFL ) ;
	if( opts < 0 )
	{
		return -1;
	}	
	opts = opts | O_NONBLOCK;
	if( fcntl( sock , F_SETFL , opts ) < 0 )
	{
		return -2;
	}
	return 0;
}

//初始化环境变量
int InitEnv(struct ServerEnv *pse)
{
	int ret = 0;
	if (pse == NULL) {
			ret = -1;
			return ret;
	}
	memset(pse,0x00,sizeof(struct ServerEnv));
 
	//初始化 监听的地址结构
	pse->listen_addr.sockaddr.sin_family = AF_INET;
	inet_aton(testip,&(pse->listen_addr.sockaddr.sin_addr));
	pse->listen_addr.sockaddr.sin_port = htons(testport);
	strcpy(pse->listen_addr.ip,testip);
	pse->listen_addr.port = testport;
	//初始化 转发的地址结构
	pse->server_addr.sockaddr.sin_family = AF_INET;
	inet_aton(testip2,&(pse->server_addr.sockaddr.sin_addr));
	pse->server_addr.sockaddr.sin_port = htons(testport2);
	strcpy(pse->server_addr.ip,testip2);
	pse->server_addr.port = testport2;
	
	pse->epoll_fds = epoll_create(EVENTSIZE);
	if (pse->epoll_fds  < 0) {
		printf("epoll_create failed [%d]",pse->epoll_fds);
		ret = -2;
		return ret;
	}	
	return ret;
}

//程序绑定监听的地址   采用epoll 的非阻塞+ET模式
int BindListenSocket(struct ServerEnv *pse)
{
	int ret = 0,nret;
	struct epoll_event	event ;
	if (pse == NULL) {
			ret = -1;
			return ret;
	}
	pse->listenfd = socket(AF_INET,SOCK_STREAM,0);
	if (pse->listenfd < 0)
	{
		printf("sock fail:[%d]\n",pse->listen_addr.sock);
		ret = -2;
		return ret;
	}
	SetReuseAddr( pse->listenfd );
	SetNonBlocking( pse->listenfd);
	nret = bind(pse->listenfd,(struct sockaddr *)&pse->listen_addr.sockaddr,sizeof(struct sockaddr));
	if (nret)
	{
		printf("bind fail:[%d]\n",nret);
		ret = -3;
		return ret;
	}
	
	nret = listen( pse->listenfd , 1024 ) ;
	if( nret )
	{
		printf("listen fail:[%d]\n",nret);
		ret = -4;
		return ret;
	}
	
	memset(&event,0x00,sizeof(struct epoll_event));
	event.data.fd = pse->listenfd;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(pse->epoll_fds,EPOLL_CTL_ADD,pse->listenfd,&event);
	
	return ret;
}

// 接受建立一个转发socket
int AcceptForwardSocket( struct ServerEnv *pse ) 
{
	int ret = 0,nret;
	if (pse == NULL) {
			ret = -1;
			return ret;
	}
	socklen_t addr_len = sizeof(struct sockaddr_in);
	struct epoll_event client_event;
	struct epoll_event server_event;
	//接受转发端口连接
	pse->listen_addr.sock = accept(pse->listenfd,(struct sockaddr *)&(pse->listen_addr.sockaddr),&addr_len);
	if ( pse->listen_addr.sock  < 0 ) {
		if (errno == EWOULDBLOCK || errno == ECONNABORTED )
			return ret;
		printf("accept  failed[%d]:errno:[%d]\n",pse->listen_addr.sock ,errno);
		ret = 1;
		return ret;
	}
	
	SetReuseAddr( pse->listen_addr.sock );
	SetNonBlocking( pse->listen_addr.sock);
		
	//创建转连的socket
	pse->server_addr.sock = socket(AF_INET,SOCK_STREAM,0);
	if (pse->server_addr.sock < 0) {
		printf("sock failed [%d] errno[%d]\n",pse->server_addr.sock,errno);
		return 1;
	}
	
	SetNonBlocking( pse->server_addr.sock);
	//连接目标网络
	nret = connect(pse->server_addr.sock,(struct sockaddr*)&(pse->server_addr.sockaddr),addr_len);
	//如果没有连接成功，设置epoll监听事件
	if (nret < 0){
		if (errno != EINPROGRESS && errno != EWOULDBLOCK) {
			printf("connect failed[%d] errno[%d]\n",nret,errno);
			close(pse->server_addr.sock);
			return 1;
		}
		memset(&server_event,0x00,sizeof(struct epoll_event));
		server_event.data.fd = pse->server_addr.sock;
		server_event.events = EPOLLOUT |EPOLLERR | EPOLLET;
		epoll_ctl(pse->epoll_fds,EPOLL_CTL_ADD,pse->server_addr.sock,&server_event);
	}
	//如果建立连接成功
	else {
		//新的连接sock加入epoll池
		memset(&client_event,0x00,sizeof(struct epoll_event));
		client_event.data.fd = pse->listen_addr.sock;
		client_event.events = EPOLLIN |EPOLLERR | EPOLLET;
		epoll_ctl(pse->epoll_fds,EPOLL_CTL_ADD,pse->listen_addr.sock,&client_event);
		
		memset(&server_event,0x00,sizeof(struct epoll_event));
		server_event.data.fd = pse->server_addr.sock;
		server_event.events = EPOLLIN |EPOLLERR | EPOLLET;
		epoll_ctl(pse->epoll_fds,EPOLL_CTL_ADD,pse->server_addr.sock,&server_event);
	}
		return 0;
}

//异步connect目标网络后的回调函数
int SetSocketConnected( struct ServerEnv *pse )
{
	socklen_t addr_len = sizeof(int),nret,ret;
	struct epoll_event client_event;
	struct epoll_event server_event;
	//要调用getsockopt看SOL_SOCKET的SO_ERROR是否为0。若为0，才表明真正的TCP层次上connect成功。
  //判断函数调用是否成功
	ret = getsockopt(pse->server_addr.sock,SOL_SOCKET,SO_ERROR,&nret,&addr_len);
	if (ret < 0 || nret) {
	    printf("getsockopt failed : %d\n",errno);
	    epoll_ctl(pse->epoll_fds,EPOLL_CTL_DEL,pse->server_addr.sock,NULL);
	    close(pse->server_addr.sock);
	    close(pse->listen_addr.sock);
	    return 1;
	}
	
	epoll_ctl(pse->epoll_fds,EPOLL_CTL_DEL,pse->server_addr.sock,NULL);
	
		//新的连接sock加入epoll池
	memset(&client_event,0x00,sizeof(struct epoll_event));
	client_event.data.fd = pse->listen_addr.sock;
	client_event.events = EPOLLIN |EPOLLERR | EPOLLET;
	epoll_ctl(pse->epoll_fds,EPOLL_CTL_ADD,pse->listen_addr.sock,&client_event);
	
	memset(&server_event,0x00,sizeof(struct epoll_event));
	server_event.data.fd = pse->server_addr.sock;
	server_event.events = EPOLLIN |EPOLLERR | EPOLLET;
	epoll_ctl(pse->epoll_fds,EPOLL_CTL_ADD,pse->server_addr.sock,&server_event);
	
	return 0;
}
//转发数据
int TransferSocketData( struct ServerEnv *pse,int sockfd ) 
{
	int in_sock;
	int out_sock;
	if (sockfd == pse->listen_addr.sock) {
		in_sock = sockfd;
		out_sock = pse->server_addr.sock;
	}
	else {
		in_sock = sockfd;
		out_sock = pse->listen_addr.sock;
	}
	while(1) { 
		//非阻塞接收数据
		memset(pse->buf,0x00,sizeof(pse->buf));
		pse->buflen = recv(in_sock,pse->buf,sizeof(pse->buf),0);
		//返回值小于0 函数执行失败 
		if (pse->buflen < 0) {
			// 循环退出的条件，表明没有接收到数据
			if (errno == EWOULDBLOCK)
				break;
			// 函数其他原因执行失败
			else
				printf("recv errno close %s:%d, close %s:%d\n",pse->listen_addr.ip,pse->listen_addr.port,pse->server_addr.ip,pse->server_addr.port);
				
				epoll_ctl(pse->epoll_fds,EPOLL_CTL_DEL,pse->listen_addr.sock,NULL);
				epoll_ctl(pse->epoll_fds,EPOLL_CTL_DEL,pse->server_addr.sock,NULL);
				close(pse->listen_addr.sock);
				close(pse->server_addr.sock);
				return 0;
		}
		// 返回值为0 ，表明对端关闭连接
		else if (pse->buflen == 0) {
				printf("recv errno close %s:%d, close %s:%d\n",pse->listen_addr.ip,pse->listen_addr.port,pse->server_addr.ip,pse->server_addr.port);
				epoll_ctl(pse->epoll_fds,EPOLL_CTL_DEL,pse->listen_addr.sock,NULL);
				epoll_ctl(pse->epoll_fds,EPOLL_CTL_DEL,pse->server_addr.sock,NULL);
				close(pse->listen_addr.sock);
				close(pse->server_addr.sock);
				return 0;
		}
		//将接收到的数据发送给对方
		#ifndef TEST
		send( out_sock ,pse->buf,sizeof(pse->buf),0 ) ;
		#endif
		#ifdef TEST
		//发送数据
		while(pse->buflen > 0) {
			int len = 0;
			len = (int)send( out_sock ,pse->buf,sizeof(pse->buf),0 ) ;
			if (len < 0 ) {
				//发送失败errno为EWOULDBLOCK时，表示写缓冲队列已满，注册epollout的回掉机制，队列由满清空时继续发送
				if (errno == EWOULDBLOCK) {
				}
				//send失败， 退出程序
				else {
					epoll_ctl(pse->epoll_fds,EPOLL_CTL_DEL,pse->listen_addr.sock,NULL);
					epoll_ctl(pse->epoll_fds,EPOLL_CTL_DEL,pse->server_addr.sock,NULL);
					close(pse->listen_addr.sock);
					close(pse->server_addr.sock);
					return 0;
				}
			}
			// 数据全部发送出去
			else if (len == pse->buflen) {
				pse->buflen = 0;
				break;
			}
			else {
				pse->buflen -= len;
				memmove(pse->buf,pse->buf+len,pse->buflen);
			}
		}
		#endif
		
	}
	return 1;
}

int ServerLoop(struct ServerEnv *pse)
{
	int ret = 0,nret,index;
	struct epoll_event	event ;
	if (pse == NULL) {
			ret = -1;
			return ret;
	}
	
	while(1) {
		//等待epoll事件发生
  	pse->sock_count = epoll_wait(pse->epoll_fds,pse->events,sizeof(pse->events),1000);
  	//如果超时没有epoll事件，就迭代
  	if (pse->sock_count == 0)
  		continue;
  	for (index = 0 ; index < pse->sock_count ; index++ ) {
  		//如果是输入事件
  		if (pse->events[index].events & EPOLLIN) {
  			//如果是转发端口事件
					if (pse->events[index].data.fd == pse->listenfd) {
						//建立转发连接
  					nret = AcceptForwardSocket( pse);
						if( nret >= 0 )
						{
							continue;
						}
						else 
						{
							printf("AcceptForwardSocket failed[%d]\r\n" , nret );
							return nret;
						}
					}
					// 是转发端口输入事件
					else {
						//转发数据
						nret = TransferSocketData( pse ,pse->events[index].data.fd) ;
						if( nret >= 0 ) {
							continue;
						}
						else {
							printf("TransferSocketData failed[%d]\r\n" , nret );
							return nret;
						}
					
					}
						
  		}
  		//如果是输出事件
  		else if (pse->events[index].events & EPOLLOUT) {
  			//如果是异步建立连接响应事件  （connect成功）
  			if (pse->events[index].data.fd == pse->server_addr.sock) {
  				printf("connect %s:%d to %s:%d\n",pse->listen_addr.ip,pse->listen_addr.port,pse->server_addr.ip,pse->server_addr.port);
  				nret = SetSocketConnected(pse);
  				if( nret > 0 ){
						continue;
					}
					else if( nret < 0 ) {
						printf( "SetSocketConnected failed[%d]\r\n" , nret );
						return nret;
					}
  			}
 
  		}
  	}
	}
}


int main()
{
	struct ServerEnv se,*pse = &se;
	InitEnv(pse);
	BindListenSocket(pse);
	ServerLoop(pse);
	return 0;
}