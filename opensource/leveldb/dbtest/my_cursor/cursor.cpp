#include <iostream>
#include "db_cxx.h"
#include <string.h>
using namespace std;

int main()
{
	Db db(NULL,0);
	//打开文件的标志
	u_int32_t flags=DB_CREATE;
	
	try  {
		//打开数据库，第一个参数是 事务指针
		//						第二个参数是  打开的数据库
		//						数据库逻辑名字
		//						数据库使用方法，设置为b树方式
		//						flags   如果文件不在就创建
		//						文件的mode  0为默认
		db.open(NULL,"my_db.db",NULL,DB_BTREE,flags,0);
		db.truncate(NULL,0,0);
		//插入一次数据
		float money=12.35;
		char des[10] = "LILI";
		//dbt 存放的数据类型
		Dbt key(&money,sizeof(money));
		Dbt value(des,strlen(des)+1);
		int ret = db.put(NULL,&key,&value,DB_NOOVERWRITE);
		cout<<"put data--"<<des<<endl;
		
		//插入第二次
		money =200;
		strcpy(des,"LUCY");
		value.set_data(des);
		value.set_size(strlen(des)+1);
		db.put(NULL,&key,&value,DB_NOOVERWRITE);
		cout<<"put data--"<<des<<endl;
		
		//插入第三次
		money =500;
		strcpy(des,"MIKE");
		value.set_data(des);
		value.set_size(strlen(des)+1);
		db.put(NULL,&key,&value,DB_NOOVERWRITE);
		cout<<"put data--"<<des<<endl;
				#if 1
		money =1500;
		strcpy(des,"MIKETEST");
		value.set_data(des);
		value.set_size(strlen(des)+1);
		db.put(NULL,&key,&value,DB_NOOVERWRITE);
		cout<<"put data--"<<des<<endl;
		#endif	
		//使用游标读取数据
		Dbc *cursor;
		db.cursor(NULL,&cursor,0);
		cout<<"open cursor"<<endl;
		//flag DB_PREV 游标指向上一个记录，返回上一条的key/value
		// LILY->LUCY->MIKE 存放
		// LILY->MIKE->LUCY 获取
		while( (ret = cursor->get(&key,&value,DB_NEXT))!= DB_NOTFOUND)
		{
			cout<<"get key--"<<*((float *)key.get_data())<<endl;
			cout<<"get value--"<<(char *)value.get_data()<<endl;
		}
		if (cursor != NULL)
			cursor->close();
		


			//删除一条记录
		db.cursor(NULL,&cursor,0);
		while((ret = cursor->get(&key,&value,DB_SET))== 0)
		{
				cout<<"delete--"<<(char *)value.get_data()<<endl;
				cursor->del(0);
		}
		if (cursor != NULL)
			{
				cursor->close();
			}
		
		//在次遍历
		db.cursor(NULL,&cursor,0);
		cout<<"open cursor"<<endl;
		while( (ret = cursor->get(&key,&value,DB_NEXT))!= DB_NOTFOUND)
		{
			cout<<"get key--"<<*((float *)key.get_data())<<endl;
			cout<<"get value--"<<(char *)value.get_data()<<endl;
		}
		if (cursor != NULL)
			cursor->close();
		
	}
	catch (DbException &e)
	{
		cout<<"dbexception"<<e.what()<<endl;
	}
	catch (std::exception &e)
		{
			cerr<<"dbexception"<<e.what();
		}
	return 0;
}