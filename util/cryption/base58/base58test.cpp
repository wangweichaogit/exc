#include <iostream>
#include "base58.h"
using namespace std;

int main(int argc,const char *argv[])
{
	if (argc != 2) {
		cout<<"usage: "<<argv[0]<<" string\n"<<endl;
		return -1;
	}
	string str1 = argv[1];
	string str2;
	string str3;
	str2 = EncodeBase58(reinterpret_cast<const unsigned char *>(str1.c_str()),str1.length());
	str3 = DecodeBase58(str2.c_str());
	
	cout<<str1.c_str()<<endl;
	cout<<str2.c_str()<<endl;
	cout<<str3.c_str()<<endl;
	return 0;
}