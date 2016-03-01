#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

void usage(int argc,const char *argv[])
{
	printf("usage:\n %s hostname port \n %s -l port\n",argv[0],argv[0]);
}

void * writefd(void *arg)
{
	pthread_detach(pthread_self());
	char buf[8096];
	int n = 0;
	int fd = *(int *)arg;
	while((n = read(STDIN_FILENO,buf,sizeof(buf))) > 0)
	{
		write(fd,buf,n);
	}
	return NULL;
}

int run(int fd)
{

	int newfd = fd;
	pthread_t pid1;
	pthread_create(&pid1,NULL,writefd,(void *)&newfd);
	char buf[8096] = { 0};
	while(1) {
		int n = read(newfd,buf,sizeof(buf));
		if (n <= 0)  {
			break;
		}	
		else  {
			write(STDOUT_FILENO,buf,n);
		}
	}
	close(fd);
	return 0;
}

int resolve(const char *inbuf,char * outbuf)
{
	char buf[4096];
	struct hostent hent;
	struct hostent *he = NULL;
	int herrno = 0;
	bzero(&hent,sizeof(hent));
	bzero(buf,4096);
	
	int ret = gethostbyname_r(inbuf,&hent,buf,sizeof(buf),&he,&herrno);
	
	if (ret == 0 && he != NULL)
	{
		sprintf(outbuf,"%s",inet_ntoa((*(struct in_addr *)he->h_addr)));
		return 0;
	}
	else
	{
		return -1;	
	}
}

int main(int argc,const char *argv[])
{
	if (argc < 3) {
		usage(argc,argv);	
		return -1;
	}
	int port = atoi(argv[2]);
	if (strcmp(argv[1],"-l") == 0)  {
		int listenfd,newfd;
		struct sockaddr_in serveraddr,recvaddr;
		socklen_t len;
		serveraddr.sin_port = htons(port);
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		listenfd = socket(AF_INET,SOCK_STREAM,0);
		if (listenfd < 0 ) {
			perror("socket");
			return -1;
		}
		int opt = 1;
		setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
		bind(listenfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
		listen(listenfd,128);
		bzero(&recvaddr,sizeof(recvaddr));
		newfd = accept(listenfd,(struct sockaddr *)&recvaddr,&len);
		run(newfd);
	
	}		
	else {
		char ipbuf[64] = { 0} ;
		int ret = resolve(argv[1],ipbuf);
		if (ret == -1) {
			printf("resolve: Name or service not known\n");
			return -1;
		}
			
		int connfd;
		struct sockaddr_in sendaddr;
		sendaddr.sin_port = htons(port);
		sendaddr.sin_family = AF_INET;
		inet_aton(ipbuf,&sendaddr.sin_addr);
		connfd = socket(AF_INET,SOCK_STREAM,0);
		if (connfd < 0 ) {
			perror("socket");
			return -1;
		}
		ret = connect(connfd,(struct sockaddr *)&sendaddr,sizeof(sendaddr));
		if (ret != 0) {
			perror("connect");
			return -2;
		}
		run(connfd);
	}
	return 0;
}
	
