/*
demo for dnsseed

getaddrinfo函数的使用
解析dns对应的所有ip地址
time： 2016/3/7
autor：  weichaowang

*/

#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <netdb.h>
#include <assert.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

void usage(int argc,const char *argv[])
{
	printf("%s [ip / hostname]\n",argv[0]);
}

bool static LookupIntern(const char *pszName,std::vector<std::string> &vIP)
{
    vIP.clear();
    struct addrinfo aiHint;
    memset(&aiHint, 0, sizeof(struct addrinfo));

    aiHint.ai_socktype = SOCK_STREAM;
    aiHint.ai_protocol = IPPROTO_TCP;
    aiHint.ai_family = AF_INET;
    aiHint.ai_flags =  AI_ADDRCONFIG ;

    struct addrinfo *aiRes = NULL;
    int nErr = getaddrinfo(pszName, NULL, &aiHint, &aiRes);
    if (nErr)
        return false;

    struct addrinfo *aiTrav = aiRes;
    while (aiTrav != NULL )
    {
        if (aiTrav->ai_family == AF_INET)
        {
            assert(aiTrav->ai_addrlen >= sizeof(sockaddr_in));
            vIP.push_back(inet_ntoa(((struct sockaddr_in*)(aiTrav->ai_addr))->sin_addr));
        }
        aiTrav = aiTrav->ai_next;
    }

    freeaddrinfo(aiRes);

    return (vIP.size() > 0);
}


int main(int argc,const char *argv[])
{
		if (argc != 2) {
		usage(argc,argv);
		return -1;
	}

	std::vector<std::string> vIP;
	if (LookupIntern(argv[1],vIP))
		{
			#if 0
			for (int i = 0 ; i < vIP.size(); i++)
			{
				printf("%s\n",vIP[i].c_str());
			}
			#else
			std::vector<std::string>::iterator it= vIP.begin();
			for ( ;it != vIP.end(); it++)
			{
				printf("%s\n",it->c_str());
			}
			#endif
		}
	else
		printf("not found\n");

	return 0;
}