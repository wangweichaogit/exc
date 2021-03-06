#include "sha1.h"
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

	CSHA1 sha1;
	unsigned char buf[sha1.OUTPUT_SIZE] = { 0};
	sha1.Reset();
	sha1.Write((const unsigned char *)str.c_str(),str.length());
	sha1.Finalize(buf);
	for (int i = 0 ; i < sizeof(buf); i++)
		printf("%02X",buf[i]);
		cout<<endl;
	return 0;
}