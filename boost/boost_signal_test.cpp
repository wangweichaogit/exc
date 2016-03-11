#include <iostream>
#include <boost/signals2.hpp>

using namespace std;
using namespace boost;

struct H
{
	void operator()()
	{
	cout<<"hello world"<<endl;
	}

};

void solt1()
{
	cout<<"hello"<<endl;
}

void solt2()
{
	cout<<"world"<<endl;
}

#if 0
int main()
{
	H hello;
	signals2::signal<void ()> sig;
	sig.connect(hello);
	sig();

	sig.connect(H());
	sig();
	sig.connect(&solt1);
	sig.connect(&solt2);
	sig();
	sig();
	sig();
	return 0;	
}
#endif

int main(void)
{
	signals2::signal<void ()> sig;
	sig.connect(1,&solt1);
	sig.connect(0,&solt2);
	sig();
	return 0;
}
