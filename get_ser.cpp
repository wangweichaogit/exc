#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h>
#include <openssl/sha.h>

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

///////////////////////////////////////////////////////////////////////////////
void myhash(const unsigned char *str, unsigned char *dest)
{
        unsigned char hash1[SHA256_DIGEST_LENGTH];
        SHA256(str,sizeof(str),hash1);
        SHA256(hash1,sizeof(hash1),dest);
}

int main(void)
{
    char mac[18] = { 0 };
 		char cpuid[32] = { 0 };
 		char buf[60] = {0 };
 		
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
    
   	sprintf(buf,"%s%s",mac,cpuid);
    unsigned char hash3[SHA256_DIGEST_LENGTH];
    myhash((const unsigned char *)buf,hash3);
    int i;
    for (i =0 ; i < SHA256_DIGEST_LENGTH;i++)
    {
                printf("%02x",hash3[i]);
    }
    printf("\n");
    return 0;
}


