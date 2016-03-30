#include <stdio.h>
#include "cfg_op.h"
int main()
{
	WriteCfgItem("cfgtest.txt","aaa","bbb",3);
	char buf[100] = { 0} ;
	int buflen = 0;
	GetCfgItem("cfgtest.txt","aaa",buf,&buflen);
	printf("len = %d, aaa = %s\n",buflen,buf);
	return 0;
}
