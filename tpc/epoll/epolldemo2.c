/*
epoll回射  epollin 和epollout用法
*/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


#define LISTENNUM 20
#define BUFLINE   4096
#define OPEN_MAX  1000

void setnonblock(int sock)
{
	int opts;
	opts = fcntl(sock,F_GETFL);
	if (opts < 0) {
		perror("fcntl error");
		return ;
	}
	opts = opts | O_NONBLOCK;
	if (fcntl(sock,F_SETFL,opts) < 0) {
		perror("fcntl error");	
		return;
	}
}

int SetReuseAddr( int sock )
{
	int	on ;
	
	on = 1 ;
	setsockopt( sock , SOL_SOCKET , SO_REUSEADDR , (void *) & on, sizeof(on) );
	
	return 0;
}
void usage()
{
	printf("./epolldemo2 portnumber\n");
}

int main(int argc,char **argv)
{
	if (argc != 2)	 {
		usage();
		return 1;
	}
	else if (atoi(argv[1]) < 0) {
		printf("portnumber error\n");
		return 1;
	}
	
	
	char buf[BUFLINE];
	int efd,listenfd,connfd,sockfd,i,ret;
	ssize_t n;
	socklen_t addrlen;
	struct epoll_event events[OPEN_MAX],event;
	struct sockaddr_in clientaddr,serveraddr;


	listenfd = socket(AF_INET,SOCK_STREAM,0);
	efd = epoll_create(10);
	setnonblock(listenfd);
	SetReuseAddr(listenfd);
	event.data.fd = listenfd;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(efd,EPOLL_CTL_ADD,listenfd,&event);

	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(argv[1]));
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(listenfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
	listen(listenfd,LISTENNUM);

	while(1) {
		ret = epoll_wait(efd,events,OPEN_MAX,1000);
		
		for (i = 0 ; i < ret; ++i) {
			if (events[i].data.fd == listenfd) {
				addrlen = sizeof(clientaddr);
				connfd = accept(listenfd,(struct sockaddr *)&clientaddr,&addrlen);
				if (connfd < 0) {
					perror("accept");
					return 2;
				}
				setnonblock(connfd);
				event.data.fd = connfd;
				event.events = EPOLLIN | EPOLLET;
				epoll_ctl(efd,EPOLL_CTL_ADD,connfd,&event);
				
			}
				
			else if (events[i].events &EPOLLIN) {
				printf("epollin\n");
				sockfd = events[i].data.fd;
				if ((n = read(sockfd,buf,BUFLINE)) < 0){
					printf("readline error\n");
				}
				else if (n == 0) {
					close(sockfd);
					events[i].data.fd = -1;
					epoll_ctl(efd,EPOLL_CTL_DEL,sockfd,NULL);
				}
				else {
					event.data.fd = sockfd;
					event.events = EPOLLOUT | EPOLLET;
					epoll_ctl(efd,EPOLL_CTL_MOD,sockfd,&event);
				}	
			}
			else if (events[i].events &EPOLLOUT) {
				printf("epollout\n");
				sockfd = events[i].data.fd;
				write(sockfd,buf,n);
				
				event.data.fd = sockfd;
				event.events = EPOLLIN | EPOLLET;
				epoll_ctl(efd,EPOLL_CTL_MOD,sockfd,&event);
			}
		}		
	}

		
}
