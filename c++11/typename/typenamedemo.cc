#include <iostream>

template<typename T>
class A {
	public:
			typedef T a_type;
		};
		
template<typename A>
class B {
	public:
		//typedef ����һ���µ��������ͣ� ��ΪA::a_type �Ǹ�ģ���type�� ����ʹ��typename
		typedef typename A::a_type b_type;
			//����typename ����뱨����������Tһ����֪
		//	typedef  A::a_type b_type;
			b_type a;
};

int main()
{
	B<A<int> > b;
	std::cout<<sizeof(b)<<std::endl;
	return 0;
}