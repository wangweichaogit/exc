#include <iostream>
#include "DES.h"
using namespace std;

int main(int argc,const char *argv[])
{
		if (argc != 2) {
		cout<<"usage: "<<argv[0]<<" string\n"<<endl;
		return -1;
	}
	EasyDES mydes(argv[1]);
	string str,str2;
	//对ab加密到str
	mydes.encrypt("ab",str);
	//str解密到str2
	mydes.decrypt(str.c_str(),str2);
	cout<<str.c_str()<<endl;
	cout<<str2.c_str()<<endl;
	return 0;
}