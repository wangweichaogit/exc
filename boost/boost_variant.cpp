#include <boost/variant.hpp>
#include <iostream>

//variant��Ч���൱��һ�����������������Է�int ���� string
//apply_visitor  ��ϵ�һ���������أ��ﵽ����variant�����ݵ�Ч������Ϊu���������͵ģ�
//�ṩ�����ڼ���ķ�ʽ

class my_visitor:public boost::static_visitor<int>
{
	public:
		int operator()(int i ) const
		{
			return i;
		}
		int operator()(const std::string str) const
			{
				return str.length();
			}
};

int main()
{
	boost::variant<int,std::string> u("hello");
	//��� hello
	std::cout<<u<<std::endl;
	int result = boost::apply_visitor(my_visitor(),u);
		//��� 5�� �൱��ִ����my_visitor my,my("hello");
		std::cout<<result<<std::endl;
			
	my_visitor my;
	std::cout<<my("hello")<<std::endl;
}