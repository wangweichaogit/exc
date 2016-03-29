#include <assert.h>  
#include <string.h>  
#include "leveldb/db.h"
#include <iostream>  
using namespace std;

/*
 使用迭代器取出值
*/

int main()
{
	
	 leveldb::DB* db;  
   leveldb::Options options;  
   options.create_if_missing = true;  
   leveldb::Status status = leveldb::DB::Open(options,"/home/wwc/leveldb/dbtest/test/testdb1", &db);  
   assert(status.ok());  
   
	leveldb::Iterator *it = db->NewIterator(leveldb::ReadOptions());
	
	for (it->SeekToFirst();it->Valid();it->Next()) {
		cout << it->key().ToString() << ": "  << it->value().ToString() << endl;
	}
	assert(it->status().ok());
	delete it;
	delete db;
	return 0;
}