/* 数据库游标和事务的玩法 */

//编译方法:安装BerkeleyDB后，gcc db04.c -ldb -o d4

/*
游标是用来连续操作一条记录的数据结构
事务是将操作数据库的一套动作原子化执行，只有全部执行成功，才会成功，否则回滚。
*/
#include <db.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

int generate_db_data(); //生成数据库
int output_db_data();   //输出数据库

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

int main(int argc, char *argv[])
{
	int ret = 0;
	ret = generate_db_data();
	if (ret != 0 ){
		print_error(ret);
		exit(ret);
	}
	ret = output_db_data();
	if (ret != 0 ){
		print_error(ret);
		exit(ret);
	}
	exit(ret);
}

int generate_db_data()
{
	int ret = 0,i;
	DB *dbp1;
	DBT key, data;
	DB_ENV *dbenv;
	u_int32_t flags;
	//生成 10 个元素的数据库
	char *stu_name[10] = {"apple01","orange02","banana03",
	"strawberry04","watermelon05","grapes06",
	"pineapple07","plum08","pear09","peach10"};
	int stu_id[10];
	for(i=0; i<10; i++)
		stu_id[i] = i+1;

	//环境
	ret = db_env_create(&dbenv, 0);
	print_error(ret);

	//环境目录"./dbtestENV"
	//flag设置:没有则创建|初始化log系统|初始化锁系统|初始化共享内存池|初始化事务子系统
	ret = dbenv->open(dbenv, "./dbtestENV", DB_CREATE|DB_INIT_LOG|DB_INIT_LOCK|
	DB_INIT_MPOOL|DB_INIT_TXN, 0);
	print_error(ret);

	ret = db_create(&dbp1, dbenv, 0);
	print_error(ret);

	flags = DB_CREATE;
	ret = dbp1->open(dbp1, NULL, "single.db", NULL, DB_BTREE, flags, 0);
	print_error(ret);

	for(i=0; i<10; i++){
		init_DBT(&key, &data);
		key.data = stu_name[i];
		key.size = strlen(stu_name[i]) + 1;
		data.data = &stu_id[i];
		data.size = sizeof(int);
		//追加写入数据库
		ret = dbp1->put(dbp1, NULL, &key, &data, DB_NOOVERWRITE);
		print_error(ret);
	}

	if(dbp1 != NULL)
		dbp1->close(dbp1, 0);

	dbenv->close(dbenv, 0);
	return 0;
}

int output_db_data()
{
	int ret = 0;
	DB *dbp1;
	DBT key, data;
	DB_ENV *dbenv;
	DBC    *cur1;       //建立游标
	DB_TXN *txn1 = NULL;//建立事务
	u_int32_t flags;

	ret = db_env_create(&dbenv, 0);
	print_error(ret);

	ret = dbenv->open(dbenv, "./dbtestENV", DB_CREATE|DB_INIT_LOG|DB_INIT_LOCK|
	DB_INIT_MPOOL|DB_INIT_TXN, 0);
	print_error(ret);

	ret = db_create(&dbp1, dbenv, 0);
	print_error(ret);

	ret = dbenv->txn_begin(dbenv, NULL, &txn1, 0);
	print_error(ret);

	flags = DB_CREATE;
	//加入事务（txn1）打开
	ret = dbp1->open(dbp1, txn1, "single.db", NULL, DB_BTREE, flags, 0);
	print_error(ret);

	ret = dbp1->cursor(dbp1, txn1, &cur1, 0);
	print_error(ret);

	init_DBT(&key, &data);
	//用游标连续获取数据打印
	while((ret = cur1->c_get(cur1, &key, &data, DB_NEXT)) == 0)
	{
		printf("The fruit = %s,\t The number = %d\n", (char *)(key.data), *(int*)(data.data));
	}
	//关闭游标
	ret = cur1->c_close(cur1);
	print_error(ret);
	//提交事务，一个原子操作完成
	ret = txn1->commit(txn1, 0);
	print_error(ret);

	if(dbp1 != NULL)
		dbp1->close(dbp1, 0);

	dbenv->close(dbenv, 0);
	return 0;
}































