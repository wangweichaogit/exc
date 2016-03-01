#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h>
///////////////////////////ip//////////////////////////////////////////
//获取地址
//返回IP地址字符串
//返回：0=成功，-1=失败
int getlocalip(char* outip)
{
    int i=0;
    int sockfd;
    struct ifconf ifconf;
    char buf[512];
    struct ifreq *ifreq;
    char* ip;
    //初始化ifconf
    ifconf.ifc_len = 512;
    ifconf.ifc_buf = buf;
 
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0)
    {
        return -1;
    }
    ioctl(sockfd, SIOCGIFCONF, &ifconf);    //获取所有接口信息
    close(sockfd);
    //接下来一个一个的获取IP地址
    ifreq = (struct ifreq*)buf;
 
    for(i=(ifconf.ifc_len/sizeof(struct ifreq)); i>0; i--)
    {
        ip = inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr);
        //排除127.0.0.1，继续下一个
        if(strcmp(ip,"127.0.0.1")==0)
        {
            ifreq++;
            continue;
        }
        strcpy(outip,ip);
        return 0;
    }
 
    return -1;
}
 
 
 ///////////////////////////////////// mac /////////////////////////////////////////////////
//获取地址
//返回MAC地址字符串
//返回：0=成功，-1=失败
//方法2，通过ifconfig得到（linux）
int get_mac(char* mac)
{
    struct ifreq tmp;
    int sock_mac;
    char mac_addr[30] = { 0 };
    sock_mac = socket(AF_INET, SOCK_STREAM, 0);
    if( sock_mac == -1)
    {
        perror("create socket fail\n");
        return -1;
    }
    memset(&tmp,0,sizeof(tmp));
    strncpy(tmp.ifr_name,"eth0",sizeof(tmp.ifr_name)-1 );
    if( (ioctl( sock_mac, SIOCGIFHWADDR, &tmp)) < 0 )
    {
        printf("mac ioctl error\n");
        return -1;
    }
    sprintf(mac_addr, "%02x%02x%02x%02x%02x%02x",
            (unsigned char)tmp.ifr_hwaddr.sa_data[0],
            (unsigned char)tmp.ifr_hwaddr.sa_data[1],
            (unsigned char)tmp.ifr_hwaddr.sa_data[2],
            (unsigned char)tmp.ifr_hwaddr.sa_data[3],
            (unsigned char)tmp.ifr_hwaddr.sa_data[4],
            (unsigned char)tmp.ifr_hwaddr.sa_data[5]
            );
    close(sock_mac);
    memcpy(mac,mac_addr,strlen(mac_addr));
    return 0;
}



//////////////////////////////////cpuid//////////////////////////////////////////////////////
//通过汇编得到cpuid

uint32_t myhtonl(uint32_t a)
{
	a=((a&0xff00ff00)>>8) | ((a&0xff00ff)<<8);
	a=((a&0xffff0000)>>16) | ((a&0xffff)<<16);
	return a;
}
int get_cpuid(char *id,size_t num)
{
		int ret = 0 ;
		if (id == NULL || num <= 0)
			{
				ret = -1;
				return ret;
			}
		unsigned long s1,s2;
		
		asm volatile   
		( "movl $0x01 , %%eax ; \n\t"
		"xorl %%edx , %%edx ;\n\t"
		"cpuid ;\n\t" 
		"movl %%edx ,%0 ;\n\t"
		"movl %%eax ,%1 ; \n\t"
		:"=m"(s1),"=m"(s2)
		);  
 		if (s1 == 0 && s2 == 0)
 				ret = -2;

 		snprintf(id,num,"%8x%8x",myhtonl(s2),myhtonl(s1));
		return ret;
}
//在root下运行
//通过系统调用得到cpuid  " sudo dmidecode -t 4 | grep ID  | sed -n '1p' | awk '$1="";1' | sed 's/ //g' "
//也可以重定向到文件，读取文件内容得到字符串
int get_cpuid_by_system(char *id,size_t num)
{
	const char *commond = "dmidecode -t 4 | grep ID  | sed -n '1p' | awk '$1=\"\";1' | sed 's/ //g' ";
	int fd[2];
	if (pipe(fd))
		{
			perror("pipe");
			return -1;
		}
	fflush(stdout);
	bzero(id,num);
	int bak_fd = dup(STDOUT_FILENO);
	int new_fd = dup2(fd[1],STDOUT_FILENO);

	if (system(commond) == 0)
	{
		read(fd[0],id,num);
	}
	dup2(bak_fd,new_fd);
	int len = strlen(id);
	if (len >1)
		id[len-1] = '\0';
	else 
		return -2;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int main(void)
{
    char ip[20];
    char mac[18];
 		char cpuid[32];
 		char cpuid2[32];
    if ( getlocalip( ip ) == 0 )
    {
        printf("本机IP地址是： %s\n", ip );
    }
    else
    {
        printf("无法获取本机IP地址\n");
    }
 
    if(get_mac(mac) == 0)
    {
        printf("本机MAC地址是： %s\n", mac);
    }
    else
    {
        printf("无法获取本机MAC地址\n");
    }
    if(get_cpuid(cpuid,32) == 0)
    {
        printf("本机cpuid是： %s\n", cpuid);
    }
    else
    {
        printf("无法获取本机cpuid\n");
    }
    return 0;
}
