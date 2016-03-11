#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>

#define PORT 8000
#define MAXLINE 10
#define OPEN_MAX 10000

int main()
{
	struct sockaddr_in s_addr,c_addr;
	int listenfd,connfd,sockfd,efd,i,j;
	ssize_t n;
	int nready,res;
	socklen_t addr_len;
	char ipstr[INET_ADDRSTRLEN];
	char buf[MAXLINE];
	int flag;
	
	struct epoll_event events[OPEN_MAX],tempevent;
	
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	int opt = 1;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	
	bzero(&s_addr,sizeof(s_addr));
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_port = htons(PORT);
	s_addr.sin_family = AF_INET;
   	
	bind(listenfd,(struct sockaddr *)&s_addr,sizeof(s_addr));

	listen(listenfd,20);
	efd = epoll_create(OPEN_MAX);
	
	tempevent.events = EPOLLIN;
	tempevent.data.fd = listenfd;
	res = epoll_ctl(efd,EPOLL_CTL_ADD,listenfd,&tempevent);
	if (res == -1)
		perror("epoll_ctl");

	while(1) {
		nready = epoll_wait(efd,events,OPEN_MAX,-1);
		if (nready == -1)
			perror("epoll_wait");
		for (i = 0 ; i < nready ; i++)
		{
			if (!events[i].events & EPOLLIN)
				continue;
			if (events[i].data.fd == listenfd)
			{
				addr_len = sizeof(c_addr);
				connfd = accept(listenfd,(struct sockaddr *)&c_addr,&addr_len);
				printf("recive data from %s at port %d \n",inet_ntop(AF_INET,&c_addr.sin_addr.s_addr,ipstr,sizeof(ipstr)),ntohs(c_addr.sin_port));
				
				flag = fcntl(connfd,F_GETFL);
				flag = O_NONBLOCK;
				fcntl(connfd,F_SETFL,flag);
				tempevent.data.fd = connfd;
				tempevent.events  = EPOLLIN | EPOLLET;
				
				res = epoll_ctl(efd,EPOLL_CTL_ADD,connfd,&tempevent);
				if (res == -1)
					perror("epoll_ctl");

			}
			else {
				sockfd = events[i].data.fd;
				while((n = read(sockfd,buf,sizeof(buf))) > 0) {
					for (j = 0 ; j < n ; j++)
						buf[j] = toupper(buf[j]);
					write(sockfd,buf,n);
				}
				if (n == 0)
				{
					res = epoll_ctl(efd,EPOLL_CTL_DEL,sockfd,NULL);
					close(sockfd);
					printf("the other size exit\n");
				}

	
			}
		}
	}
	





}
