/* 数据库存取数据 */

// 文件执行后，当前目录产生数据库文件demo2.db
// 编译方法:安装BerkeleyDB后，gcc db02.c -ldb -o d2
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#define DATABASE "./demo2.db"
int main()
{
	DB *dbp;
	DBT key, data;
	int ret,t_ret;
	u_int32_t env_flags;

	//创建数据库句柄
	if ((ret = db_create(&dbp, NULL, 0)) != 0){
		fprintf(stderr, "db_create: %s\n", db_strerror(ret));
		exit (1);
	}

	//打开数据库
	if ((ret = dbp->open(dbp, NULL, DATABASE, NULL, DB_BTREE, DB_CREATE, 0664)) != 0){
		dbp->err(dbp, ret, "%s", DATABASE);
		exit (1);
	}
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data)); 
	key.data = "sport";//填写键
	key.size = sizeof("sport");
	data.data = "football";//填写值
	data.size = sizeof("football");

	//写入数据:覆盖写入
	if ((ret = dbp->put(dbp, NULL, &key, &data, 0)) == 0)
		printf("db: %s: key stored.\n", (char *)key.data);
	else
		dbp->err(dbp, ret, "DB->put");

	//获取数据
	memset(&data, 0, sizeof(data)); 
	if ((ret = dbp->get(dbp, NULL, &key, &data, 0)) == 0)
		printf("db: %s: key retrieved: data was %s.\n", (char *)key.data, (char *)data.data);
	else
		dbp->err(dbp, ret, "DB->get");

	//删除数据
	if((ret = dbp->del(dbp, NULL, &key, 0)) == 0)
		printf("db: %s: key was deleted.\n", (char *)key.data);
	else
		dbp->err(dbp, ret, "DB->del");

	//关闭数据库：数据库成功关闭后，数据才会真的写入磁盘
	if (dbp != NULL)
		dbp->close(dbp, 0);

	return 0;
}






