#include "sha256.h"
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

	CSHA256 sha256;
	unsigned char buf[sha256.OUTPUT_SIZE] = { 0};
	sha256.Reset();
	sha256.Write((const unsigned char *)str.c_str(),str.length());
	sha256.Finalize(buf);
	for (int i = 0 ; i < sizeof(buf); i++)
		printf("%02X",buf[i]);
		cout<<endl;
	return 0;
}