#include "hash.h"
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

	CHash256 hash256;
	unsigned char buf[hash256.OUTPUT_SIZE] = { 0};
	hash256.Reset();
	hash256.Write((const unsigned char *)str.c_str(),str.length());
	hash256.Finalize(buf);
	for (int i = 0 ; i < sizeof(buf); i++)
		printf("%02X",buf[i]);
		cout<<endl;
	return 0;
}