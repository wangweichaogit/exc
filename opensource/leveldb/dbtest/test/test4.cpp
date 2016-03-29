/*
leveldb中的事务，  使一批操作要么都成功，要么都失败
leveldb::WriteBatch
需要加头文件 #include <leveldb/write_batch.h>
*/
#include <assert.h>  
#include <string.h>  
#include "leveldb/db.h"
#include "leveldb/write_batch.h"
#include <iostream>  

int main()
{
	//打开数据库
	leveldb::DB* db;  
	leveldb::Options options;  
	options.create_if_missing = true;  
	leveldb::Status status = leveldb::DB::Open(options,"/home/wwc/leveldb/dbtest/test/testdb1", &db);
  //设置原子更新
	leveldb::WriteBatch *activeBatch =  new leveldb::WriteBatch();
	std::string key="testkey";  
	std::string key2="key2";  
	std::string value = "testvalue";  
	activeBatch->Put(key,value);
	activeBatch->Delete(key2);
	leveldb::Status s = db->Write(leveldb::WriteOptions(),activeBatch);
	delete activeBatch;
	delete db;
	return 0;
}