#include "../univalue.h"
#include <iostream>
#include <vector>
using namespace std;
//转换为字符串输出 使用write(2) 方法 转换为varray
void printUniValue(UniValue uv)
{
	std::string strPrint;
	if (uv.isNull())
		strPrint = "";
	else if (uv.isStr())
		strPrint = uv.get_str();
	else
		strPrint = uv.write(2);
	std::cout<<strPrint.c_str()<<std::endl;
}
int main()
{
	
	/////////////////////////  json
	UniValue uv(UniValue::VOBJ);
	printUniValue(uv);

	uv.pushKV("hello","world");
	uv.pushKV("uv",100);
	uv.pushKV("uvx",true);
	printUniValue(uv);
	
	UniValue uv2(UniValue::VOBJ);
	uv2.pushKV("uv2","uv222");
	if (uv.pushKV("uv2",uv2))
			std::cout<<uv2.type()<<std::endl;
	
	UniValue uv3 = find_value(uv,"uv2");
	std::cout<<uv3["uv2"].get_str()<<std::endl;

	
	std::cout<<uv["uv"].get_int()<<std::endl;
	
	UniValue uv33 = find_value(uv,"uvx");
		std::cout<<uv33.type()<<std::endl;
	std::cout<<uv33.get_int()<<std::endl;
	////////////////////////////// string
	UniValue uv30(UniValue::VARR);
	uv30.push_back("test string");
		uv30.push_back("test string 2");
			std::cout<<uv30.type()<<std::endl;
	std::cout<<uv30.write(2).c_str()<<std::endl;
		#if 1
	/////////////////////////////    ARRAY
	
	UniValue a(UniValue::VARR);
	a.push_back("string1");
	a.push_back("string2");
	a.push_back("string3");
	a.push_back("string4");
	
	cout<<"a :"<<a.write(2).c_str()<<endl;
	UniValue b(UniValue::VOBJ);
	b.pushKV("strings",a);
	b.pushKV("test string",uv30);
	UniValue c = find_value(b,"strings");
	std::cout<<"strings:"<<c.write(2).c_str()<<std::endl;
		
	#if 0
	vector<string>st = b.getKeys();
	cout<<"{"<<endl;
	for (int i = 0 ; i < st.size();i++) {
		cout<<st[i].c_str()<<":";
		cout<<find_value(b,(st[i]).c_str()).write(2).c_str()<<endl;
	}
	cout<<"}"<<endl;
	#endif
	cout<<b.write(1).c_str()<<endl;
	#endif
	UniValue uv5(true);
	std::cout<<std::boolalpha<<uv5.get_bool()<<std::endl;
		
		
	UniValue uv6(180);
	std::cout<<uv6.get_int()<<std::endl;
		
	return 0;
}