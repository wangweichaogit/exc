/* 带环境变量的数据库 */

// 文件执行后，DB_HOME会产生环境文件和数据库文件
// 编译方法:安装BerkeleyDB后，gcc db03.c -ldb -o d3

/*数据库的两个意思：1.宏观上指数据库系统，包括数据库主系统，
                    数据库子系统，数据库文件，数据库日志等等。
                    2.微观上就是指用户的数据库文件（存放数据的文件）
				    本例中为"demo.db"
					EGD为"wallet.dat"
数据库的环境：一个数据库系统，常常被多个进程或者线程共享。
              为共享数据库系统的所有进程/应用/线程，建立一个home环境目录，
			  存放缓存、日志文件等等。
			  本例为"/home/insight/dbhome"
			  EGD为"/home/xxx/.E-Gold"
设置了环境变量的数据库，所有函数的相对路径以后都已环境home为基础。
即"wallet.dat"实际上是"/home/xxx/.E-Gold/wallet.dat"
数据库环境变量： DB_HOME*/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
int main()
{
	DB_ENV *myEnv; //数据库环境变量：用来设置（数据库目录等）
	DB     *dbp;   //数据库句柄
	DBT key, data; //数据
	int ret,t_ret;
	u_int32_t env_flags;

	//创建数据库环境句柄，并初始化
	ret = db_env_create(&myEnv, 0);
	if (ret != 0)
	{
		fprintf(stderr, "Error creating env handle: %s\n", db_strerror(ret));
		return -1;
	}
	//环境不存在则创建 | 启用共享内存缓存池子系统（必须有）
	env_flags = DB_CREATE | DB_INIT_MPOOL;

	//打开一个环境（参2是所有函数相对路径的基础）
	ret = myEnv->open(myEnv,"/home/insight/dbhome",env_flags,0);
	if (ret != 0)
	{
		fprintf(stderr, "Environment open failed: %s", db_strerror(ret));
		return -1;
	}

	//创建数据库句柄
	if ((ret = db_create(&dbp, myEnv, 0)) != 0)
	{
		fprintf(stderr, "db_create: %s\n", db_strerror(ret));
		exit (1);
	}

	//打开数据库
	if ((ret = dbp->open(dbp, NULL, "demo.db", NULL, DB_BTREE, DB_CREATE, 0664)) != 0)
	{
		dbp->err(dbp, ret, "%s", "demo.db");
		exit (1);
	}
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data)); 
	key.data = "sport";//设置数据库 键数据
	key.size = sizeof("sport");
	data.data = "football";//设置数据库 值数据
	data.size = sizeof("football");

	//写入数据（追加写入）： NOOVERWRITE
	if ((ret = dbp->put(dbp, NULL, &key, &data, DB_NOOVERWRITE)) == 0)
		printf("db: %s: key stored.\n", (char *)key.data);
	else 
		dbp->err(dbp, ret, "DB->put");

	//读取数据
	if ((ret = dbp->get(dbp, NULL, &key, &data, 0)) == 0)
		printf("db: %s: key retrieved: data was %s.\n", (char *)key.data, (char *)data.data);
	else
		dbp->err(dbp, ret, "DB->get");

/*
	//删除数据
	if((ret = dbp->del(dbp, NULL, &key, 0)) == 0)
		printf("db: %s: key was deleted.\n", (char *)key.data);
	else
		dbp->err(dbp, ret, "DB->del");
*/

	//关闭数据库：数据库成功关闭后，数据才会真的写入磁盘
	if (dbp != NULL)
		dbp->close(dbp, 0);

	//关闭环境（必须关闭），必须确保先关闭了数据库。
	if (myEnv != NULL)
		myEnv->close(myEnv, 0);
	return 0;
}






