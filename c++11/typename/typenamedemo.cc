#include <iostream>

template<typename T>
class A {
	public:
			typedef T a_type;
		};
		
template<typename A>
class B {
	public:
		//typedef 定义一个新的数据类型， 因为A::a_type 是个模板的type， 必须使用typename
		typedef typename A::a_type b_type;
			//不加typename 则编译报错，编译器对T一无所知
		//	typedef  A::a_type b_type;
			b_type a;
};

int main()
{
	B<A<int> > b;
	std::cout<<sizeof(b)<<std::endl;
	return 0;
}