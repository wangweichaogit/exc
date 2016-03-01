/*
G5������ת��demo
���г������8080�˿ڻ�ת�ӵ�8090�˿ڡ�
 192.168.119.250::8080  -> 192.168.119.250::8090
 
 ���ؾ��������������һ�����ӣ������½�һ�����ӣ����ӵ� ��ַ���е�һ����ַ�ϣ�������Ϣ��ת��
 
 test����:
 1.ִ��ת������ ./epollG5 
 2.nc ģ��server��  nc -l 192.168.119.250 8090
 3.nc ģ��client��  nc 192.168.119.250 8080
 
 
 EPOLLIN �� EPOLLOUT
 epollout������ ������sendʱ�� д��������������
 								������connectʱ  ������
 EPOLLOUT���������������ط�(�첽�ص�)�� 1 ������connect����Ŀ��ʱ��û������������ʱ ����epoll_ctl����fd�ϵ�epoll
 out�¼����¼��������лص�   ��������connect + epollout�ص� + getsockopt����ֵ�ж�connect�Ƿ������ϣ�
 Ҳ���ԣ�������connect + select������д���� + getsockopt����ֵ�ж�connect�Ƿ������ϣ�
 2. send���ݷ���ʧ��  epoll_ctl��sockfd����Ϊepollout��ͨ��epoll�Ļص������������ݣ����������ݺ�epoll_ctl�ٽ�socketfd
 ����Ϊepollin������
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

#define BUFSIZE 4096      //����buff��С
#define EVENTSIZE  10000    //epoll�¼����ϴ�С

const char * testip = "127.0.0.1";
const char * testip2 = "127.0.0.1";
int  testport = 8080;
int testport2 = 8090;

//	�ڵ��ַ �� sock������
struct Node{
	struct sockaddr_in sockaddr;
	char ip[64];
	int port;
	int sock;
};

//����Ļ��������ṹ
struct ServerEnv{
	int 				epoll_fds;      //epoll������
	struct 			epoll_event events[EVENTSIZE];   //epoll �¼�����
	
	int         listenfd;       //  ����sockket
	struct Node listen_addr;   	//  ��ת���ĵ�ַ�ṹ
	struct Node server_addr;		//   ת���ĵ�ַ�ṹ
	char   			buf[BUFSIZE];    //��Ϣ�շ�buf
	int					buflen;        //buf����
	int 				sock_count;    //���ص�epoll�¼���Ŀ
};

// ���õ�ַ����
int SetReuseAddr( int sock )
{
	int	on ;
	
	on = 1 ;
	setsockopt( sock , SOL_SOCKET , SO_REUSEADDR , (void *) & on, sizeof(on) );
	
	return 0;
}

//�׽�������Ϊ������
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

//��ʼ����������
int InitEnv(struct ServerEnv *pse)
{
	int ret = 0;
	if (pse == NULL) {
			ret = -1;
			return ret;
	}
	memset(pse,0x00,sizeof(struct ServerEnv));
 
	//��ʼ�� �����ĵ�ַ�ṹ
	pse->listen_addr.sockaddr.sin_family = AF_INET;
	inet_aton(testip,&(pse->listen_addr.sockaddr.sin_addr));
	pse->listen_addr.sockaddr.sin_port = htons(testport);
	strcpy(pse->listen_addr.ip,testip);
	pse->listen_addr.port = testport;
	//��ʼ�� ת���ĵ�ַ�ṹ
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

//����󶨼����ĵ�ַ   ����epoll �ķ�����+ETģʽ
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

// ���ܽ���һ��ת��socket
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
	//����ת���˿�����
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
		
	//����ת����socket
	pse->server_addr.sock = socket(AF_INET,SOCK_STREAM,0);
	if (pse->server_addr.sock < 0) {
		printf("sock failed [%d] errno[%d]\n",pse->server_addr.sock,errno);
		return 1;
	}
	
	SetNonBlocking( pse->server_addr.sock);
	//����Ŀ������
	nret = connect(pse->server_addr.sock,(struct sockaddr*)&(pse->server_addr.sockaddr),addr_len);
	//���û�����ӳɹ�������epoll�����¼�
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
	//����������ӳɹ�
	else {
		//�µ�����sock����epoll��
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

//�첽connectĿ�������Ļص�����
int SetSocketConnected( struct ServerEnv *pse )
{
	socklen_t addr_len = sizeof(int),nret,ret;
	struct epoll_event client_event;
	struct epoll_event server_event;
	//Ҫ����getsockopt��SOL_SOCKET��SO_ERROR�Ƿ�Ϊ0����Ϊ0���ű���������TCP�����connect�ɹ���
  //�жϺ��������Ƿ�ɹ�
	ret = getsockopt(pse->server_addr.sock,SOL_SOCKET,SO_ERROR,&nret,&addr_len);
	if (ret < 0 || nret) {
	    printf("getsockopt failed : %d\n",errno);
	    epoll_ctl(pse->epoll_fds,EPOLL_CTL_DEL,pse->server_addr.sock,NULL);
	    close(pse->server_addr.sock);
	    close(pse->listen_addr.sock);
	    return 1;
	}
	
	epoll_ctl(pse->epoll_fds,EPOLL_CTL_DEL,pse->server_addr.sock,NULL);
	
		//�µ�����sock����epoll��
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
//ת������
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
		//��������������
		memset(pse->buf,0x00,sizeof(pse->buf));
		pse->buflen = recv(in_sock,pse->buf,sizeof(pse->buf),0);
		//����ֵС��0 ����ִ��ʧ�� 
		if (pse->buflen < 0) {
			// ѭ���˳�������������û�н��յ�����
			if (errno == EWOULDBLOCK)
				break;
			// ��������ԭ��ִ��ʧ��
			else
				printf("recv errno close %s:%d, close %s:%d\n",pse->listen_addr.ip,pse->listen_addr.port,pse->server_addr.ip,pse->server_addr.port);
				
				epoll_ctl(pse->epoll_fds,EPOLL_CTL_DEL,pse->listen_addr.sock,NULL);
				epoll_ctl(pse->epoll_fds,EPOLL_CTL_DEL,pse->server_addr.sock,NULL);
				close(pse->listen_addr.sock);
				close(pse->server_addr.sock);
				return 0;
		}
		// ����ֵΪ0 �������Զ˹ر�����
		else if (pse->buflen == 0) {
				printf("recv errno close %s:%d, close %s:%d\n",pse->listen_addr.ip,pse->listen_addr.port,pse->server_addr.ip,pse->server_addr.port);
				epoll_ctl(pse->epoll_fds,EPOLL_CTL_DEL,pse->listen_addr.sock,NULL);
				epoll_ctl(pse->epoll_fds,EPOLL_CTL_DEL,pse->server_addr.sock,NULL);
				close(pse->listen_addr.sock);
				close(pse->server_addr.sock);
				return 0;
		}
		//�����յ������ݷ��͸��Է�
		#ifndef TEST
		send( out_sock ,pse->buf,sizeof(pse->buf),0 ) ;
		#endif
		#ifdef TEST
		//��������
		while(pse->buflen > 0) {
			int len = 0;
			len = (int)send( out_sock ,pse->buf,sizeof(pse->buf),0 ) ;
			if (len < 0 ) {
				//����ʧ��errnoΪEWOULDBLOCKʱ����ʾд�������������ע��epollout�Ļص����ƣ������������ʱ��������
				if (errno == EWOULDBLOCK) {
				}
				//sendʧ�ܣ� �˳�����
				else {
					epoll_ctl(pse->epoll_fds,EPOLL_CTL_DEL,pse->listen_addr.sock,NULL);
					epoll_ctl(pse->epoll_fds,EPOLL_CTL_DEL,pse->server_addr.sock,NULL);
					close(pse->listen_addr.sock);
					close(pse->server_addr.sock);
					return 0;
				}
			}
			// ����ȫ�����ͳ�ȥ
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
		//�ȴ�epoll�¼�����
  	pse->sock_count = epoll_wait(pse->epoll_fds,pse->events,sizeof(pse->events),1000);
  	//�����ʱû��epoll�¼����͵���
  	if (pse->sock_count == 0)
  		continue;
  	for (index = 0 ; index < pse->sock_count ; index++ ) {
  		//����������¼�
  		if (pse->events[index].events & EPOLLIN) {
  			//�����ת���˿��¼�
					if (pse->events[index].data.fd == pse->listenfd) {
						//����ת������
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
					// ��ת���˿������¼�
					else {
						//ת������
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
  		//���������¼�
  		else if (pse->events[index].events & EPOLLOUT) {
  			//������첽����������Ӧ�¼�  ��connect�ɹ���
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