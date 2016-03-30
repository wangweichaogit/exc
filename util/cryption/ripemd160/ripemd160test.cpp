#include "ripemd160.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

int main(int argc,const char *argv[])
{
			if (argc != 2) {
		cout<<"usage: "<<argv[0]<<" string\n"<<endl;
		return -1;
	}
	string str(argv[1]);
	CRIPEMD160 ripemd160;
	unsigned char buf[ripemd160.OUTPUT_SIZE] = { 0};
	ripemd160.Reset();
	ripemd160.Write((const unsigned char *)str.c_str(),str.length());
	ripemd160.Finalize(buf);
	for (int i = 0 ; i < sizeof(buf); i++)
		printf("%02X",buf[i]);
		cout<<endl;
	return 0;
}