#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define PORT  9000


int main()
{
    int sfd,cfd;
    struct sockaddr_in saddr,caddr;
    socklen_t  addrlen;
    char buf[1024];
    char buf1[1024];
    int num;
 
    saddr.sin_port = htons(PORT);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
 
 
    sfd = socket(AF_INET,SOCK_STREAM,0);


 		struct linger ling;
 		ling.l_onoff = 1;
 		ling.l_linger = 0;
 		setsockopt(sfd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));

		
		int opt =1;
		setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    bind(sfd,(struct sockaddr *)&saddr,sizeof(saddr));
 
    listen(sfd,64);
    addrlen = sizeof(caddr);
 
    cfd = accept(sfd,(struct sockaddr *)&caddr,&addrlen);
 
 
    printf("client ip = %s, port = %d\n",inet_ntop(AF_INET,&(caddr.sin_addr.s_addr),buf1,sizeof(buf1)),ntohs(caddr.sin_port));
 		printf("sleep 1 s\n");
    sleep(1);
 
		#if 0
		shutdown(cfd,SHUT_WR);
		#endif

		close(cfd);

		close(sfd);
    return 0;
}