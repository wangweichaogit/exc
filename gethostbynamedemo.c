#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void usage()
{
	printf("./gethostbynamedemo [ip / hostname]\n");
}
int main(int argc,char **argv)
{
	if (argc != 2) {
		usage();
		return -1;
	}
	char buf[4096];
	struct hostent hent;
	struct hostent *he = NULL;
	int herrno = 0;
	bzero(&hent,sizeof(hent));
	bzero(buf,4096);
	
	int ret = gethostbyname_r(argv[1],&hent,buf,sizeof(buf),&he,&herrno);
	
	if (ret == 0 && he != NULL)
	{
		printf("h_name = %s\n",he->h_name);
		printf("ret = %d, buf=%s,herrno=%d\n",ret,buf,herrno);
		printf("%s\n",inet_ntoa((*(struct in_addr *)he->h_addr)));
		return 1;
	}
	else
	{
		perror("error");
		return 0;	
	}
	return 0;
}
