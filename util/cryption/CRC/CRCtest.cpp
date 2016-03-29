#include "CRC.h"
#include <iostream>
using namespace std;

int main(int argc,char *argv[])
{
	if (argc != 2) {
	cout<<"usage: "<<argv[0]<<" string\n"<<endl;
	return -1;
	}
	string str(argv[1]);
	unsigned int ret = CRC32::calc(str);
		cout<<ret<<endl;
		return 0;
}