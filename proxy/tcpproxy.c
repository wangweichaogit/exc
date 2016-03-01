/*
program:	tcpproxy		
time: 		2016/2/16
description:	tcp代理   每条代理连接一个进程
1.接受一个连接   fork出一个子进程   ，在子进程中完成转发
2.循环接收新的连接
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/wait.h>

int proxy_port;
struct sockaddr_in forwardaddr;

//设置地址重用
int SetReuseAddr( int sock )
{
    int    on ;

    on = 1 ;
    setsockopt( sock , SOL_SOCKET , SO_REUSEADDR , (void *) & on, sizeof(on) );

    return 0;
}

//usage
void usage()
{
	printf("usage:<proxy-port> <host> <port-number> \n");
}

//对fd进行代理
void do_proxy(int sockfd)
{
	int forwardsockfd,ret;
	int len;
	char buf[4096];
	fd_set rdfdset;
	if ((forwardsockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	  printf("failed to create socket to host\n");
	ret = connect(forwardsockfd,(struct sockaddr *) &forwardaddr, sizeof(forwardaddr));
	if (ret != 0) {
		strcpy(buf,"cant connect to peer\n");
		write(sockfd,buf,strlen(buf));
		close(sockfd);
		return;
	}
	while (1)
	{

		FD_ZERO(&rdfdset);
		FD_SET(sockfd,&rdfdset);
		FD_SET(forwardsockfd,&rdfdset);
		if (select(FD_SETSIZE,&rdfdset,NULL,NULL,NULL) < 0)
			printf("select failed\n");

		if (FD_ISSET(sockfd,&rdfdset)) 
		{
			if ((len = read(sockfd,buf,sizeof(buf))) <= 0) {
			  break; 
			}
			write(forwardsockfd,buf,len);

		}

		if(FD_ISSET(forwardsockfd,&rdfdset)) 
		{
			if((len = read(forwardsockfd,buf,sizeof(buf))) <= 0) {
			  break;
			}
			write(sockfd,buf,len);

		}
	}
	close(forwardsockfd);
	close(sockfd);
}

//设置命令行参数
int  prase_args(int argc,char **argv)
{
		if (argc != 4) {
			usage();
			return -1;
		}
		proxy_port = atoi(argv[1]);
		forwardaddr.sin_family = AF_INET;
		forwardaddr.sin_port =	htons(atoi(argv[3]));
		inet_aton(argv[2],&forwardaddr.sin_addr);
		return 0;
}

//绑定本机socket接口
int BindListenSocket(int *listenfd)
{
	struct sockaddr_in seraddr;
	
	bzero(&seraddr,sizeof(seraddr));
	seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(proxy_port);
	
	if((*listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
	  fputs("failed to crate server socket\r\n", stderr);
	  return -1;
	}
	if(bind(*listenfd, (struct sockaddr*)&seraddr, sizeof(seraddr)) < 0)
	{
	  fputs("failed to bind server socket to specified/r/n", stderr);
	  return -1;
	}
	listen(*listenfd, 5);
	SetReuseAddr( *listenfd);
	return 0;
}

//信号捕捉函数 回收子进程
void reap_status(int sig)
{
  while( waitpid(0,NULL,WNOHANG) > 0);
}
//设置信号捕捉
void setsignal()
{	
	struct sigaction sig;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	sig.sa_handler = reap_status;
	sigaction(SIGCLD,&sig,NULL);
}


int main(int argc,char **argv)
{
	if (prase_args(argc,argv) < 0 )
		return -1;
	struct sockaddr_in clientaddr;
	int listenfd,newsockfd;
	socklen_t addrlen;
	
	BindListenSocket(&listenfd);
	setsignal();
	
	while(1) {
		addrlen = sizeof(struct sockaddr_in);
		newsockfd = accept(listenfd,(struct sockaddr *)&clientaddr,&addrlen);
		// 被信号打断时候  重新accept
		if (newsockfd < 0 && errno == EINTR)
			continue;
			//其他原因接受链接失败
		if (newsockfd < 0 ) {
			printf("failed accept newconnect\n");
			return -1;
		}	
		//子进程
		if (fork() == 0)	{
			//子进程不需要监听
			close(listenfd);
			//子进程中完成信息转发
			do_proxy(newsockfd);
			return 0;
		}
		//父进程关闭新接socket
		close(newsockfd);
	}
	return 0;
}