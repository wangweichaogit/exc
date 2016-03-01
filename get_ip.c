#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>


unsigned int pnseed[] = 
{
	0x0ce5bb25,0x0ee5bb25,0xe4540905
};

int main()
{
	struct in_addr ip;
	for (int i = 0; i < 3 ; i++)
	{
		memcpy(&ip,&pnseed[i],sizeof(ip));
		printf("%s\n",inet_ntoa(ip));
	}
	return 0;
}
