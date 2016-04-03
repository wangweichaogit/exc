#include "../univalue.h"
#include <iostream>
#include <vector>

void printUniValue(UniValue uv)
{
	std::string strPrint;
	if (uv.isNull())
		strPrint = "";
	else if (uv.isStr())
		strPrint = uv.get_str();
	else
		strPrint = uv.write(2);
		std::cout<<"//////////////"<<std::endl;
	std::cout<<strPrint.c_str()<<std::endl;
		std::cout<<"\ \\\\\\\\\\\\"<<std::endl;
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
	uv2.push_back(uv);
	printUniValue(uv);
	printUniValue(uv2);	
	
	////////////////////////////// string
	UniValue uv3(UniValue::VSTR);
	uv3.push_back("test string");
	uv3.push_back("test string 2");
	printUniValue(uv3);

	return 0;
}