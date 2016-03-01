#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#define SERVER_PORT 8080 

int main()
{

	int client_fd = 0;
	struct sockaddr_in server_addr;
	socklen_t len = sizeof(server_addr);
	//创建套接字
	if ((client_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
		perror("socket");
		
	//设置套接字为非阻塞
	int flags = fcntl(client_fd,F_GETFL,0);
	if (fcntl(client_fd,F_SETFL,flags|O_NONBLOCK) == -1)
		{	
			printf("fcntl error \n");
			close(client_fd);
			return -1;
		}
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	//连接本地ip
	//server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	inet_aton("127.0.0.1",&server_addr.sin_addr.s_addr);
	//inet_aton("47.88.18.66",&server_addr.sin_addr.s_addr);
	//非阻塞连接 ，没有建立连接返回-1,并设置errno为EINPROGRESS
	if (connect(client_fd,(struct sockaddr *)&server_addr,len) == -1)
		{
			int nErr = errno;
			//当errno为下面三种时，使用select监听
			if (nErr == EINPROGRESS || nErr == EWOULDBLOCK || nErr ==EINVAL )
				{
					//设置监听时间
					struct timeval timeout;
					timeout.tv_sec = 2;
					timeout.tv_usec = 0;
					
					//设置监听集合
					fd_set fdset;
					FD_ZERO(&fdset);
					FD_SET(client_fd,&fdset);
					//select返回非阻塞socket描述符可写则表明连接建立成功，如果select返回非阻塞socket描述符既可读又可写则表明连接出错
					int nret = select(client_fd+1,NULL,&fdset,NULL,&timeout);
					//连接超时
					if (nret == 0)
						{
							printf("connect timeout\n");
							close(client_fd);
							return -2;
						}
						//select 失败
					if (nret == -1)
						{
							printf("select for connect failed :%d\n",errno);
							close(client_fd);
							return -3;
						}
						//没有失败，此时不能认为tcp层次的connect成功
						//要调用getsockopt看SOL_SOCKET的SO_ERROR是否为0。若为0，才表明真正的TCP层次上connect成功。
						socklen_t nretsize = sizeof(nret);
						//判断函数调用是否成功
						if (getsockopt(client_fd,SOL_SOCKET,SO_ERROR,&nret,&nretsize)== -1)
							{
								printf("getsockopt failed : %d\n",errno);
								close(client_fd);
								return -4;
							}
						//连接不成功
						if (nret != 0)
							{
								printf("connect failed after select :connect refused\n");
								close(client_fd);
								return -5;
							}
							
							//连接成功
							printf("connect success\n");
							printf("sleep 3s\n");
							//send(client_fd,"hello world\n",12,MSG_DONTWAIT);
							sleep(3);
							
							#if 1
							while(1) {	

									   char pchBuf[0x10000];
									  int nBytes = recv(client_fd, pchBuf, sizeof(pchBuf), MSG_DONTWAIT);
									  if (nBytes == 0)
									  	{
									  		printf("socket closed\n");
									  		close(client_fd);
												return -3;
									  	}
									  else if (nBytes > 0)
									  	{
									  		printf("recv data\n");
												continue;
									  	}
									  	else if (errno == EAGAIN && nBytes == -1)
									  	{
									  		printf("no message recived \n");
									  		sleep(1);
												continue;
									  	}
									else {
										printf("recv error %d\n",errno);
										return -4;
									}
									  	sleep(1);
								
								}
						}
						#endif
		}
		else
			{
				printf("connect failed\n");
				return -6;
			}
}
