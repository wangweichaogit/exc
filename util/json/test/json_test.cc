#include "../json_spirit_value.h"
#include "../json_spirit_writer_template.h"
#include "../json_spirit_reader_template.h"
#include "../json_spirit.h"
#include <iostream>

using namespace std;
using namespace json_spirit;
//value  
// Object ���� vector<Pair(,)>
Object testObject()
{
	Object ob,ob2;
	ob.push_back(Pair("ob","world"));
	ob.push_back(Pair("ob", (int) 1 ));
	ob.push_back(Pair("ob", true ));
		
	ob2.push_back(Pair("ob2",23));
	ob.push_back(Pair("ob2",ob2));
	return ob;
}


//ֱ�ӷ����ַ���
Value testString()
{
	return "hello ,world";
}

void testArray(const Array &params)
{
	for (int i = 0 ; i < params.size() ; i++)
	{
		cout<<params[i].get_str()<<endl;
	}
}
int main()
{
	Value vtest,vtest2;
	string output; 
	//////////////////////////  ����object
	vtest = testObject();
	output = write_string(vtest,true);
	cout<<output.c_str()<<endl;
	
	//////////////////////////// ����string
	vtest2 = testString();
	output = write_string(vtest2,true);
	cout<<output.c_str()<<endl;
	
	////////////////////////   ����array
	Array array;
	array.push_back("test array");
	array.push_back(vtest2);
	testArray(array);
	
	////////////////////// ��ȡ����ֵ
	const Object & obj = testObject();
	const Value &result = find_value(obj,"ob2");
		output = write_string(result,true);
	cout<<output.c_str()<<endl;

	//////////////////////////////  ���� read_string
	Value val;
	read_string(output,val);
	output = write_string(val,true);
	cout<<output.c_str()<<endl;
	
	return 0;
}