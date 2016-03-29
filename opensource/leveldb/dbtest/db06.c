/* 游标和事务的综合玩法 */

//编译方法:安装BerkeleyDB后，gcc db06.c -ldb -o d6
/*
在一个环境下（共享）建立两个数据库dbp1,dbp2。
给两个数据库分别建立两个各自的游标cur1,cur2。
数据库dbp1在事务系统（txn）下运行，执行原子操作。
数据库dbp2是普通操作。
*/
#include <db.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
void print_error(int ret)
{
	if(ret != 0)
		printf("ERROR: %s\n",db_strerror(ret));
}
void init_DBT(DBT * key, DBT * data)
{
	memset(key, 0, sizeof(DBT));
	memset(data, 0, sizeof(DBT));
}

int main(void)
{
	DB *dbp1, *dbp2;
	DBT key, data;
	DB_ENV *dbenv;
	DBC *cur1, *cur2;
	DB_TXN *txn = NULL;
	u_int32_t flags;
	int ret;
	char *fruit = "apple";
	int number = 15;
	char *orange = "orange";
	int number2 = 10;
//自定义结构，存放数据
typedef struct customer
{
	int  c_id;
	char name[10];
	char address[20];
	int  age;
} CUSTOMER;

	CUSTOMER cust;
	cust.c_id = 1;
	strncpy(cust.name, "sunwukong", 9);
	strncpy(cust.address, "huaguoshan", 19);
	cust.age = 32;

	ret = db_env_create(&dbenv, 0);
	print_error(ret);

	ret = dbenv->open(dbenv, "./dbtestENV", DB_CREATE|DB_INIT_LOG|DB_INIT_LOCK|DB_INIT_MPOOL|DB_INIT_TXN, 0);
	print_error(ret);

	ret = db_create(&dbp1, dbenv, 0);
	print_error(ret);

	//开始事务操作
	ret = dbenv->txn_begin(dbenv, NULL, &txn, 0);
	print_error(ret);

	flags = DB_CREATE;
	//dbp1在操作的single.db数据库
	ret = dbp1->open(dbp1, txn, "single.db", NULL, DB_BTREE, flags, 0);
	print_error(ret);

	ret = dbp1->cursor(dbp1, txn, &cur1, 0);
	print_error(ret);

	init_DBT(&key, &data);
	key.data = fruit;
	key.size = strlen(fruit) + 1;
	data.data = &number;
	data.size = sizeof(int);
	//dbp1存入水果数据
	ret = dbp1->put(dbp1, txn, &key, &data, DB_NOOVERWRITE);
	print_error(ret);

	init_DBT(&key, &data);
	key.data = orange;
	key.size = strlen(orange) + 1;
	data.data = &number2;
	data.size = sizeof(int);
	ret = dbp1->put(dbp1, txn, &key, &data, DB_NOOVERWRITE);
	print_error(ret);
	//dbp1读取数据
	while((ret = cur1->c_get(cur1, &key, &data, DB_NEXT)) == 0)
	{
		printf("The fruit = %s, The number = %d\n", (char *)(key.data), *(int*)(data.data));
	}

	ret = db_create(&dbp2, dbenv, 0);
	print_error(ret);

	flags = DB_CREATE;
	//dbp2操作的是"complex.db"数据库
	ret = dbp2->open(dbp2, NULL, "complex.db",NULL, DB_HASH, flags, 0);
	print_error(ret);

	ret = dbp2->cursor(dbp2, NULL, &cur2, 0);
	print_error(ret);

	init_DBT(&key, &data);
	key.size = sizeof(int);
	key.data = &(cust.c_id);
	data.data = &cust;
	data.ulen = sizeof(CUSTOMER);
	data.flags = DB_DBT_USERMEM;
	//给dbp2添加客户数据
	ret = dbp2->put(dbp2, NULL, &key, &data,DB_NOOVERWRITE);
	print_error(ret);

	//输出dbp2客户数据
	while((ret = cur2->c_get(cur2, &key, &data, DB_NEXT)) == 0)
	{
		printf("c_id = %d name = %s address = %s age = %d\n",
		cust.c_id, cust.name, cust.address, cust.age);
	}

	ret = cur1->c_close(cur1);
	print_error(ret);

	ret = cur2->c_close(cur2);
	print_error(ret);

	//两个数据库都读写完了数据，才提交dbp1的事务
	ret = txn->commit(txn, 0);
	print_error(ret);

	if(dbp1 != NULL)
		dbp1->close(dbp1, 0);

	if(dbp2 != NULL)
		dbp2->close(dbp2, 0);

	dbenv->close(dbenv, 0);
	return 0;
}



























