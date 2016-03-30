#include "InetAddress.h"
#include <iostream>
using namespace std;

int main(int argc,char *argv[])
{
	if (argc != 2) {
		cout<<argv[0]<<"hostname"<<endl;
		return -1;
	}
	vector<InetAddress> v1;
	v1 = InetAddress::resolveAll(argv[1]);
	for (int i = 0 ; i < v1.size(); i++)
		cout<<v1[i].toIpPort().c_str()<<endl;
	return 0;
}
