#include <boost/variant.hpp>
#include <iostream>

//variant功效：相当于一个联合性容器，可以放int 或者 string
//apply_visitor  配合第一个（）重载，达到访问variant中数据的效果（因为u是联合类型的）
//提供编译期检验的方式

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
	//输出 hello
	std::cout<<u<<std::endl;
	int result = boost::apply_visitor(my_visitor(),u);
		//输出 5， 相当于执行了my_visitor my,my("hello");
		std::cout<<result<<std::endl;
			
	my_visitor my;
	std::cout<<my("hello")<<std::endl;
}