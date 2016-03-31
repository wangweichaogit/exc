#include <iostream>
#include "DES.h"
using namespace std;

int main(int argc,const char *argv[])
{
		if (argc != 3) {
		cout<<"usage: "<<argv[0]<<"flags"<<" string\n"<<endl;
		return -1;
	}
	EasyDES mydes(argv[1]);
	string str,str2;
	//对ab加密到str
	mydes.encrypt(argv[2],str);
	//str解密到str2
	mydes.decrypt(str.c_str(),str2);
	cout<<argv[1]<<str.c_str()<<endl;
	cout<<str2.c_str()<<endl;
	return 0;
}
