#include <iostream>
#include <boost/bind.hpp> 
#include <boost/shared_ptr.hpp>
using namespace std; 
using namespace boost;

int g(int a,int b)
{
	a++;
	return a+b;
}

struct X
{
	bool f(int a)
	{return a==0?false:true;}
};
int main(void)
{
/*  bind(g,1,2)()  ==  g(1,2)     */
#if 0
	cout<<bind(g,1,2)()<<endl;
	cout<<g(2,3)<<endl;
#endif
	
#if 0
	int x = 8;
	cout<<bind(less<int>(),_1,9)(x)<<endl;

#endif
	

	X x;
	shared_ptr<X> p(new X);
	int i = 5;

	cout<<boolalpha <<bind(&X::f,ref(x),_1)(1)<<endl;
	cout<<boolalpha <<bind(&X::f,&x,_1)(1)<<endl;
	cout<<boolalpha <<bind(&X::f,x,_1)(1)<<endl;
	cout<<boolalpha <<bind(&X::f,p,_1)(1)<<endl;
	
	return 0;
}
