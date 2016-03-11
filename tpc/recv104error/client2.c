/*
����Server A������Process X������һ��socket M���������Server B�����Process Y�� Socket N��TCPЭ���������ˣ���ô��������֪����2����������RST����

��1��Xû��close socket���˳��ˣ�Ȼ��Y������M send���ݣ�A���ں˾ͻᷢ��RST �� socket N��

��2��X������SO_LINGER������l_onoff ��0�� l_linger Ϊ0��������A close socket M��ʱ��Ҳ�ᷢ��RST��socket N��

��socket N�յ���RST��select�Ľ��Ϊsocket�ɶ�����

��a��������ʱ�����recv������-1��errnoΪECONNRESET������ٴε���recv������-1��errnoΪEPIPE��ͬ�²���EPIPE�źţ�

��b��������ʱ�����send������-1��errnoΪEPIPE��ͬʱ�����SIGPIPE�ź�

--------------------------------------------------------------------
���1��
	server��û��close������
	client��  ��д��recv
	��server��ʹ��close ���� shutdown write ������errno��
���2��
	server����SO_LINGER���ر�ʱʹ��close�ر���������û����shutdown��shutdown��Ҳ���ᷢ��ECONNRESET��
	client  ֱ��recv    ���ȡ��errno
*/


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

#define SERVER_PORT 9000

int main()
{

	int client_fd = 0;
	struct sockaddr_in server_addr;
	socklen_t len = sizeof(server_addr);
	//�����׽���
	if ((client_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
		perror("socket");
		
	int opt =1;
	setsockopt(client_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	//�����׽���Ϊ������
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
	//���ӱ���ip
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	//inet_aton("52.7.207.121",&server_addr.sin_addr.s_addr);
	//inet_aton("47.88.18.66",&server_addr.sin_addr.s_addr);
	//���������� ��û�н������ӷ���-1,������errnoΪEINPROGRESS
	if (connect(client_fd,(struct sockaddr *)&server_addr,len) == -1)
		{
			int nErr = errno;
			//��errnoΪ��������ʱ��ʹ��select����
			if (nErr == EINPROGRESS || nErr == EWOULDBLOCK || nErr ==EINVAL )
				{
					//���ü���ʱ��
					struct timeval timeout;
					timeout.tv_sec = 2;
					timeout.tv_usec = 0;
					
					//���ü�������
					fd_set fdset;
					FD_ZERO(&fdset);
					FD_SET(client_fd,&fdset);
					//select���ط�����socket��������д��������ӽ����ɹ������select���ط�����socket�������ȿɶ��ֿ�д��������ӳ���
					int nret = select(client_fd+1,NULL,&fdset,NULL,&timeout);
					//���ӳ�ʱ
					if (nret == 0)
						{
							printf("connect timeout\n");
							close(client_fd);
							return -2;
						}
						//select ʧ��
					if (nret == -1)
						{
							printf("select for connect failed :%d\n",errno);
							close(client_fd);
							return -3;
						}
						//û��ʧ�ܣ���ʱ������Ϊtcp��ε�connect�ɹ�
						//Ҫ����getsockopt��SOL_SOCKET��SO_ERROR�Ƿ�Ϊ0����Ϊ0���ű���������TCP�����connect�ɹ���
						socklen_t nretsize = sizeof(nret);
						//�жϺ��������Ƿ�ɹ�
						if (getsockopt(client_fd,SOL_SOCKET,SO_ERROR,&nret,&nretsize)== -1)
							{
								printf("getsockopt failed : %d\n",errno);
								close(client_fd);
								return -4;
							}
						//���Ӳ��ɹ�
						if (nret != 0)
							{
								printf("connect failed after select :connect refused\n");
								close(client_fd);
								return -5;
							}
							
							//���ӳɹ�
							printf("connect success\n");
						
		}
		else
			{
				printf("connect success\n");
				return -6;
			}
			printf("sleep 3s\n");
			sleep(3);
						

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
}
