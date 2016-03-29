/* 数据库的创建，打开 */

// 文件执行后，当前目录下产生数据库文件demo1.db
// 编译方法:安装BerkeleyDB后，gcc db01.c -ldb -o d1

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#define DATABASE "./demo.db" //数据库文件名称

int main()
{ 
    DB *dbp; //数据库句柄
	int ret;

	//创建数据库
    if ((ret = db_create(&dbp, NULL, 0)) != 0) {
        fprintf(stderr, "db_create: %s\n", db_strerror(ret));
        exit (1);
    }
	
	//打开数据库
    if ((ret = dbp->open(dbp, NULL, DATABASE, NULL, DB_BTREE, DB_CREATE,
                         0664)) != 0) {
        dbp->err(dbp, ret, "%s", DATABASE);
        exit (1);
    }
}
