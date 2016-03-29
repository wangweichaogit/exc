#include <iostream>
#include "MD5.h"
#include <stdio.h>
using namespace std;

int main(int argc,const char *argv[])
{
		if (argc != 2) {
		cout<<"usage: "<<argv[0]<<" string\n"<<endl;
		return -1;
	}
	#if 0
	cout<<"usage1----------------------"<<endl;
	string str(argv[1]);
	MD5 md5;
	md5.init();
	md5.update((unsigned char *)(str.c_str()),str.length());
	md5.final();
	unsigned char output[16] = { 0 };
	md5.digest(output);
	int i = 0;
	for (i = 0 ; i < strlen((const char *)output); i++)
		printf("%x",output[i]);
		printf("\n");
	
	cout<<"usage2----------------------"<<endl;
	#endif
	MD5Ext md52(argv[1],strlen(argv[1]));
	#if 0
	char output2[16] = { 0};
	md52.digest(output2);
		for (i = 0 ; i < strlen((const char *)output); i++)
		printf("%x",output2[i]);
		printf("\n");
		#endif
	std::string hexdigest;
	md52.digest(hexdigest);
	cout<<hexdigest.c_str()<<endl;
	
	#if 0
	cout<<"usage3----------------------"<<endl;
	char output3[16] = { 0};
	md5digest(output3,argv[1],strlen(argv[1]));
		for (i = 0 ; i < strlen((const char *)output); i++)
		printf("%x",output3[i]);
		printf("\n");
		#endif
	return 0;
}
