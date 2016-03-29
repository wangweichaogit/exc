/* 综合练习：数据库的读，写，删除，更新 */

//编译方法:安装BerkeleyDB后，gcc db05.c -ldb -o mydb
#include <db.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

int fun_main(int ,int ,int);//主函数
int generate_db_data(); //生成数据库
int output_db_data();   //输出数据库
int operate_db_data();  //对数据库的操作选择
int DB_Insert(); //插入
int DB_Delete(); //删除
int DB_Modify(); //修改
int DB_Seach();  //查找

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
	int ret;
	int generate, output, process;
	ret = 0; generate = 0; output = 0; process = 0;
	if(argc != 2)
	{
		printf("argv error! Please run the program with parameter.\n");
		printf("Sample : ./fun -g, 产生数据库\n");
		printf("         ./fun -o, 遍历输出数据库\n");
		printf("         ./fun -p, 操作数据库\n");
		exit(ret);
	}
	while(*argv[1] != '\0')
	{
		if(*argv[1] == '-'){
			argv[1]++;
			continue;
		}
		else if(*argv[1] == 'g')
			generate = 1;
		else if(*argv[1] == 'o')
			output = 1;
		else if(*argv[1] == 'p')
			process = 1;
		argv[1]++;
	}

	ret = fun_main(generate, output, process);
	exit(ret);

}

int fun_main(int generate, int output, int process)
{
	int ret = 0;
	if(generate == 1 && process != 1){
		//DB产生
		ret = generate_db_data();
	}

	if(output == 1 && process != 1){
		//DB输出
		ret = output_db_data();
	}

	if(process == 1)
	{
		//DB操作
		ret = operate_db_data();
	}

	if(generate == 0 && output == 0 && process == 0)
	{
		printf("parameter input error, please run again!\n");
		printf("Sample : ./fun -o\n");
	}

	return ret;
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

int operate_db_data()
{
	int ret = 0,sel = 0;
	printf("What do you want to do :\n1.Insert\n2.Delete\n3.Modify\n4.Search\n");
	scanf("%d",&sel);
	switch(sel)
	{
		case 1:
		DB_Insert();
		break;
		case 2:
		DB_Delete();
		break;
		case 3:
		DB_Modify();
		break;
		case 4:
		DB_Seach();
		break;
		default:
		printf("please select from 1-4! \n");
	}
	return ret;
}

int DB_Insert()
{
	int ret = 0;
	DB *dbp1;
	DBT key, data;
	DB_ENV *dbenv;
	u_int32_t flags;
	char name[20] = {0};
	int number;
	printf("Please input the data and value :");
	scanf("%s%d",name,&number);
	printf("You insert data : The fruit =\"%s\", the number = \"%d\"\n",name,number);

	ret = db_env_create(&dbenv, 0);
	print_error(ret);

	ret = dbenv->open(dbenv, "./dbtestENV", DB_CREATE|DB_INIT_LOG|DB_INIT_LOCK|
	DB_INIT_MPOOL|DB_INIT_TXN, 0);
	print_error(ret);

	ret = db_create(&dbp1, dbenv, 0);
	print_error(ret);

	flags = DB_CREATE;
	//不带事务打开
	ret = dbp1->open(dbp1, NULL, "single.db", NULL, DB_BTREE, flags, 0);
	print_error(ret);

	init_DBT(&key, &data);
	key.data = name;
	key.size = strlen(name) + 1;
	data.data = &number;
	data.size = sizeof(int);

	//插入数据
	ret = dbp1->put(dbp1, NULL, &key, &data, DB_NOOVERWRITE);
	print_error(ret);

	if(ret == 0)
		printf("INSERT SUCCESS!\n");
	else
		printf("INSERT FAILED!\n");

	if(dbp1 != NULL)
		dbp1->close(dbp1, 0);

	dbenv->close(dbenv, 0);
	return ret;
}

int DB_Delete()
{
	int ret=0;
	char name[20];
	DB *dbp1;
	DBT key, data;
	DB_ENV *dbenv;
	u_int32_t flags;
	printf("Which data do you want to delete: ");
	scanf("%s",name);
	printf("You want to delete the data is :\"%s\"\n",name);

	ret = db_env_create(&dbenv, 0);
	print_error(ret);

	ret = dbenv->open(dbenv, "./dbtestENV", DB_CREATE|DB_INIT_LOG|DB_INIT_LOCK|
	DB_INIT_MPOOL|DB_INIT_TXN, 0);
	print_error(ret);

	ret = db_create(&dbp1, dbenv, 0);
	print_error(ret);

	flags = DB_CREATE;
	ret = dbp1->open(dbp1, NULL, "single.db", NULL, DB_BTREE, flags, 0);
	print_error(ret);

	init_DBT(&key, &data);
	key.data = name;
	key.size = strlen(name) + 1;

	ret = dbp1->del(dbp1, NULL, &key, 0);
	print_error(ret);

	if(ret == 0)
		printf("DELETE SUCCESS!\n");
	else
		printf("DELETE FAILED!\n");

	if(dbp1 != NULL)
		dbp1->close(dbp1, 0);

	dbenv->close(dbenv, 0);
	return ret;
}

int DB_Modify()
{
	int ret = 0;
	DB *dbp1;
	DBT key, data;
	DB_ENV *dbenv;
	DBC *cur1;
	DB_TXN *txn1 = NULL;
	u_int32_t flags;
	char name_old[20], name_new[20];
	int number;

	printf("Which data do you want to modify(key name): ");
	scanf("%s",name_old);
	printf("The new data is :");
	scanf("%s%d",name_new, &number);

	ret = db_env_create(&dbenv, 0);
	print_error(ret);

	ret = dbenv->open(dbenv, "./dbtestENV", DB_CREATE|DB_INIT_LOG|DB_INIT_LOCK|
	DB_INIT_MPOOL|DB_INIT_TXN, 0);
	print_error(ret);

	ret = db_create(&dbp1, dbenv, 0);
	print_error(ret);

	//事务开始
	ret = dbenv->txn_begin(dbenv, NULL, &txn1, 0);
	print_error(ret);

	flags = DB_CREATE;
	//事务打开数据库
	ret = dbp1->open(dbp1, txn1, "single.db", NULL, DB_BTREE, flags, 0);
	print_error(ret);

	ret = dbp1->cursor(dbp1, txn1, &cur1, 0);
	print_error(ret);
	init_DBT(&key, &data);
	key.data = name_old;
	key.size = strlen(name_old) + 1;
	//修改逻辑：
	//1.先获取，并删除原数据
	while((ret = cur1->c_get(cur1, &key, &data, DB_SET)) == 0)
	{
		printf("The data you want to modify is: The fruit = %s,\t The number = %d\n", (char *)(key.data),*(int*)(data.data));

		ret = cur1->c_del(cur1, 0);
		break;
	}

	init_DBT(&key, &data);
	key.data = name_new;
	key.size = strlen(name_new) + 1;
	data.data = &number;
	data.size = sizeof(int);

	//2.然后再插入新数据
	ret = dbp1->put(dbp1, txn1, &key, &data, DB_NOOVERWRITE);
	if(ret == 0)
		printf("MODIFY SUCCESS!\n");
	else
		printf("MODIFY FAILED!\n");

	ret = cur1->c_close(cur1);
	print_error(ret);

	//提交事务，原子操作完成
	ret = txn1->commit(txn1, 0);
	print_error(ret);
	
	if(dbp1 != NULL)
		dbp1->close(dbp1, 0);

	dbenv->close(dbenv, 0);
	return 0;
}

int DB_Seach()
{
	int ret = 0;
	DB *dbp1;
	DBT key, data;
	DB_ENV *dbenv;
	DBC *cur1;
	DB_TXN *txn1 = NULL;
	u_int32_t flags;
	char name[20];
	printf("Which data do you want to search: ");
	scanf("%s",name);
	printf("You want to search the data is :\"%s\"\n",name);
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

	ret = dbp1->open(dbp1, txn1, "single.db", NULL, DB_BTREE, flags, 0);
	print_error(ret);
	ret = dbp1->cursor(dbp1, txn1, &cur1, 0);
	print_error(ret);

	init_DBT(&key, &data);
	key.data = name;
	key.size = strlen(name) + 1;
	while((ret = cur1->c_get(cur1, &key, &data, DB_SET)) == 0)
	{
		printf("The fruit = %s,\t The number = %d\n", (char *)(key.data), *(int*)(data.data));
		break;
	}

	if(ret != 0)
		printf("The data which you want cannot found.\n");

	ret = cur1->c_close(cur1);
	print_error(ret);
	ret = txn1->commit(txn1, 0);
	print_error(ret);
	if(dbp1 != NULL)
		dbp1->close(dbp1, 0);

	dbenv->close(dbenv, 0);
	return 0;
}
























































