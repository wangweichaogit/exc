#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;
void printHash(unsigned char *md, int len)  
{  
    int i = 0;  
    for (i = 0; i < len; i++)  
    {  
        printf("%02x", md[i]);  
    }  
  
    printf("\n");  
} 


#if 0

//sha256加密
const  char *str="hello";
int main()
{
	unsigned char md[SHA256_DIGEST_LENGTH];
	SHA256((const unsigned char *)str,sizeof(str),md);
	printHash(md,SHA256_DIGEST_LENGTH);
	return 0;
}
#endif
//hash
void myhash(const unsigned char *str, unsigned char *dest)
{
	unsigned char hash1[SHA256_DIGEST_LENGTH];
	SHA256(str,sizeof(str),hash1);
	SHA256(hash1,sizeof(hash1),dest);
}
int main(int argc,char**argv)
{
	if (argc != 2)
	{
		printf("input ./sha256 string\n");
		return -1;
	}
	printf("%d\n",SHA256_DIGEST_LENGTH);
	unsigned char hash3[SHA256_DIGEST_LENGTH];
	myhash((const unsigned char *)argv[1],hash3);
	printHash(hash3,SHA256_DIGEST_LENGTH);
	return 0;
}

