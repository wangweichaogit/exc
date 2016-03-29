#include <iostream>
#include <stdio.h>
#include "base64.h"


int main()
{
	char buf[100];
	memcpy(buf,"a111sdds",sizeof("a111sdds"));
	ZBase64 base64;
	std::string str = base64.Encode((unsigned char *)buf,strlen(buf));
	std::cout<<str.c_str()<<std::endl;
	
	int i,buflen = 0;
	std::string str2 = base64.Decode(str.c_str());
	std::cout<<str2.c_str()<<std::endl;

	return 0;
}