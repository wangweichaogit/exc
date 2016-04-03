#include "../univalue.h"
#include <iostream>
#include <vector>
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
	uv.pushKV("uv",1);
	uv.pushKV("uv",true);
	
	UniValue uv2;
	if (uv2.push_back(uv))
			std::cout<<uv2.type()<<std::endl;
	printUniValue(uv);
	printUniValue(uv2);	
	
	////////////////////////////// string
	UniValue uv3(UniValue::VARR);
	uv3.push_back("test string");
		uv3.push_back("test string 2");
			std::cout<<uv3.type()<<std::endl;
	std::cout<<uv3.write(2).c_str()<<std::endl;

	/////////////////////////////    ARRAY
	
	UniValue uv4(UniValue::VARR);
	uv4.push_back(uv);
		uv4.push_back(uv2);
		uv4.push_back(uv3);
		
		for (int i = 0; i < uv4.size() ; i++) {
			printUniValue(uv4[i]);
	}
		#if 0
	////////////////////////////////////
		UniValue uv4(UniValue::VNUM);
		uv4.push_back(10);
		std::cout<<uv4.getValStr().c_str()<<std::endl;
	#endif
	return 0;
}