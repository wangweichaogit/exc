#include "sha512.h"
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

	CSHA512 sha512;
	unsigned char buf[sha512.OUTPUT_SIZE] = { 0};
	sha512.Reset();
	sha512.Write((const unsigned char *)str.c_str(),str.length());
	sha512.Finalize(buf);
	for (int i = 0 ; i < sizeof(buf); i++)
		printf("%02X",buf[i]);
		cout<<endl;
	return 0;
}