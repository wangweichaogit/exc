#include <stdio.h>
#include "log.h"

int main()
{
	int ret = 0;
	//level0 不写入
	WJSMLOG_LOG(__FILE__,__LINE__,LogLevel[0],ret,"this is first test");
	WJSMLOG_LOG(__FILE__,__LINE__,LogLevel[1],ret,"this is second test");
	WJSMLOG_LOG(__FILE__,__LINE__,LogLevel[2],ret,"this is three test");
	WJSMLOG_LOG(__FILE__,__LINE__,LogLevel[3],ret,"this is four test");
	WJSMLOG_LOG(__FILE__,__LINE__,LogLevel[4],ret,"this is five test");
	return 0;
}

