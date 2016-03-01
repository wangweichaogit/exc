#include <iostream>
#include <boost/function.hpp>
#include <boost/bind.hpp>

using namespace std;
using namespace boost;

int func(int a, int b)
{
	return a+b;
}
int main()
{
	function2<int ,int,int> a;
	a = func;
	cout<<a(1,2)<<endl;
	return 0;
}
