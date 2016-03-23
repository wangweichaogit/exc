
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef struct Node {
	int		data;
	struct Node *next;
}SLIST;

SLIST	*SList_Creat();
int		SList_Print(SLIST *pHead);
int		SList_NodeInsert(SLIST *pHead, int x , int y);
int		SList_NodeDel(SLIST *pHead, int y);
int		SList_Destory(SLIST *pHead);

int		SList_Destory(SLIST *pHead)
{
	int		rv = 0;
	SLIST	*p = NULL, *pTmp = NULL;
	
	if (pHead == NULL)
	{
		rv = -1;
		printf("func SList_Destory() err:%d", rv);
		return rv;
	}
	//循环遍历链表
	p = pHead;
	while (p)
	{
		//缓存后续结点
		pTmp = p->next;
		//删除当前结点
		free(p);
		//当前结点指针后移
		p = pTmp;
	}

	return rv;
}

//动态创建带有头结点链表
//循环接受用户输入，malloc新结点，新结点入链表
//代码有点丑
//1 业务逻辑有内存泄露发生
//2 返回值为链表首地址占用了错误码
SLIST *SList_Creat()
{
	//int	rv = 0;
	int		data = 0;
	SLIST *pHead = NULL, *pCur = NULL, *pM = NULL;

	//创建头结点并初始化
	pHead = (SLIST *)malloc(sizeof(SLIST));
	if (pHead == NULL)
	{
		return NULL;
	}
	pHead->data = 0;
	pHead->next = NULL;

	//等待用户输入
	printf("\nplease enter the data of node:(-1:quit) ");
	scanf("%d", &data);

	//循环判断用户输入
	pCur = pHead;
	while (data != -1)
	{
		//创建新结点并赋值
		pM = (SLIST *)malloc(sizeof(SLIST));
		if (pM == NULL)
		{
			return NULL;
		}
		pM->data = data;
		pM->next = NULL;

		//新结点入链表
		pCur->next = pM;
		
		//当前指针变量后移一个单位
		pCur = pM;

		//等待用户输入
		printf("\nplease enter the data of node:(-1:quit) ");
		scanf("%d", &data);
	}

	return pHead;
}

//工作经验提升1:1年
//当有函数有内存或malloc资源申请时，函数结构要求  
//一个入口一个出口
//打桩思想灵活应用：并行开发，提高开发速度
//野指针：指针变量所指的内存空间已释放，但是指针没置成NULL，会对后续程序产生影响 
//程序健壮性提升 实战经验
SLIST *SList_Creat2()
{
	int		rv = 0;
	int		data = 0;
	SLIST *pHead = NULL, *pCur = NULL, *pM = NULL;

	//创建头结点并初始化
	pHead = (SLIST *)malloc(sizeof(SLIST));
	if (pHead == NULL)
	{
		rv = -1;
		printf("func SList_Creat2() err:%d", rv);
		goto End;
	}
	pHead->data = 0;
	pHead->next = NULL;

	//等待用户输入
	printf("\nplease enter the data of node:(-1:quit) ");
	scanf("%d", &data);

	//循环判断用户输入
	pCur = pHead;
	while (data != -1)
	{
		//创建新结点并赋值
		pM = (SLIST *)malloc(sizeof(SLIST));
		if (pM == NULL)
		{
			rv = -2;
			printf("func SList_Creat2() err:%d", rv);
			goto End;
		}
		pM->data = data;
		pM->next = NULL;

		//新结点入链表
		pCur->next = pM;

		//当前指针变量后移一个单位
		pCur = pM;

		//等待用户输入
		printf("\nplease enter the data of node:(-1:quit) ");
		scanf("%d", &data);
	}

End:
	
	if (rv != 0)
	{
		SList_Destory(pHead); //野指针
		pHead = NULL;
	}
	return pHead;
}

//工作经验提升 2年  返回值为链表首地址占用了错误码
/*
	func1调用func2，func2中分配内存malloc，传递给fun1的方法有两种 
	1 return 方法
	2 二级指针做函数参数

	//二级指针典型用法：
	//输入　char *argv[];
	//输出: 实参取地址，赋值给形参，在被调用函数里面，通过*形参（*p） = 
	//指针做函数参数的精髓
*/
//正确进行接口库的封装和设计  2年工作经验
int SList_Creat3(SLIST **headnode)
{
	int		rv = 0;
	int		data = 0;
	SLIST *pHead = NULL, *pCur = NULL, *pM = NULL;

	//创建头结点并初始化
	pHead = (SLIST *)malloc(sizeof(SLIST));
	if (pHead == NULL)
	{
		rv = -1;
		printf("func SList_Creat2() err:%d", rv);
		goto End;
	}
	pHead->data = 0;
	pHead->next = NULL;

	//等待用户输入
	printf("\nplease enter the data of node:(-1:quit) ");
	scanf("%d", &data);

	//循环判断用户输入
	pCur = pHead;
	while (data != -1)
	{
		//创建新结点并赋值
		pM = (SLIST *)malloc(sizeof(SLIST));
		if (pM == NULL)
		{
			rv = -2;
			printf("func SList_Creat2() err:%d", rv);
			goto End;
		}
		pM->data = data;
		pM->next = NULL;

		//新结点入链表
		pCur->next = pM;

		//当前指针变量后移一个单位
		pCur = pM;

		//等待用户输入
		printf("\nplease enter the data of node:(-1:quit) ");
		scanf("%d", &data);
	}

End:

	if (rv != 0)
	{
		SList_Destory(pHead); //野指针
		pHead = NULL;
	}
	//铁律2：*p间接赋值是指针存在的最大意义
	//*p 在等号的左边 修改所指内存变量的值（赋值）
	//*p 在等号的右边，取值
	//理解 *p2++ = *p1++ //
	*headnode = pHead;
	return rv;
}

//工作经验：3年
//企业级财富库 通用链表接口开发与设计
//做项目讲
//思想：
int SList_Creat4(void **headnode)
{
	return 0;
}

//铁律二: *p间接赋值是指针存在的最大意义
/*
//成立三个条件 
	1)定义一个实参 ，2)定义一个形参
		int rv = 0;		int *p = NULL; p = &rv  *p
		void *p = NULL;	void **p2		p2 = &p1; *p2 = 0x1 
	3)实参地址赋给形参，*形参，去修改实参的值
*/

void Test01()
{
	int rv = 0;

	int *p = 0x1;
	rv = 1; //直接赋值

	p = &rv;
	*p = 2; //间接赋值  //*p 在等号的左边 修改所指内存变量的值（赋值）
	printf("%d", rv); //
}

void Test02()
{
	void *p1 = NULL;
	void **p2 = NULL;
	p1 = 0x1; //直接赋值
	
	p2 = &p1;
	
	*p2 = 0x2; //间接赋值 去修改p1的值 由0x1==>0x2
	
	printf("%x", p1); //*p间接赋值
}


int		SList_Print(SLIST *pHead)
{
	int		rv = 0;
	SLIST	*p = NULL;

	if (pHead == NULL)
	{
		rv = -1;
		printf("func SList_Print() err:%d", rv);
		return rv;
	}
	p = pHead->next;
	printf("\nBegin ");
	while(p)
	{
		printf("%d\t", p->data);
		p = p->next;
	}
	printf("End\n");

	return rv;
}

int		SList_NodeInsert(SLIST *pHead, int x , int y)
{
	int		rv = 0;
	SLIST	*pM = NULL, *pCur = NULL, *pPre = NULL;

	if (pHead == NULL)
	{
		rv = -1;
		printf("func SList_NodeInsert() err.\n", rv);
		return rv;
	}

	//malloc新结点并赋值
	pM = (SLIST *)malloc(sizeof(SLIST));
	if (pM == NULL)
	{
		rv = -2;
		printf("func SList_NodeInsert() err.\n", rv);
		return rv;
	}
	pM->data = y;
	pM->next = NULL;

	//初始化前趋结点、当前结点指针变量
	pPre = pHead;
	pCur = pHead->next;

	//遍历链表，查找符合条件的结点
	//找出pre结点和当前结点
	while(pCur)
	{
		if (pCur->data == x)
		{
			break;
		}
		//前趋结点后移，当前节点后移
		pPre = pCur;
		pCur = pCur->next;
	}
	
	//在当前结点之前插入新节点
	//新结点连接当前结点
	pM->next = pPre->next;
	//前趋结点连接新结点，完成新结点入链表
	pPre->next = pM;

	return rv;
}


int		SList_NodeDel(SLIST *pHead, int y)
{
	int		rv = 0;
	SLIST	*pCur = NULL, *pPre = NULL;

	if (pHead == NULL)
	{
		rv = -1;
		printf("func SList_NodeInsert() err.\n", rv);
		return rv;
	}

	//初始化前趋结点、当前结点指针变量
	pPre = pHead;
	pCur = pHead->next;

	//遍历链表，查找符合条件的结点
	//找出pre结点和当前结点
	while(pCur)
	{
		if (pCur->data == y)
		{
			break;
		}
		//前趋结点后移，当前节点后移
		pPre = pCur;
		pCur = pCur->next;
	}

	//没有符合条件的结点，报错
	if (pCur == NULL)
	{
		rv = -2;
		printf("func SList_NodeDel() err:%d", rv);
		return rv;
	}
	//删除结点
	//前趋结点连接后续结点
	pPre->next = pCur->next;
	free(pCur);
	
	return rv;
}

int main()
{
	int		rv = 0;
	SLIST	*pHead = NULL;

	Test();
	/*
	pHead = SList_Creat2();
	if (pHead == NULL)
	{
		printf("func SList_Creat2() err\n");
		return rv;
	}
	printf("func SList_Creat() ok\n");
	*/

	rv = SList_Creat3(&pHead);
	if (rv != 0)
	{
		printf("func SList_Creat3() err:%d\n", rv);
		goto End;
	}
	printf("func SList_Creat3() ok\n");

	rv = SList_Print(pHead);
	if (rv != 0)
	{
		printf("func SList_Print() err:%d\n", rv);
		goto End;
	}
	printf("func SList_Print() ok\n");

	rv = SList_NodeInsert(pHead, 20, 19);
	if (rv != 0)
	{
		printf("func SList_NodeInsert() err:%d\n", rv);
		goto End;
	}
	rv = SList_Print(pHead);
	if (rv != 0)
	{
		printf("func SList_Print() err:%d\n", rv);
		goto End;
	}

	rv = SList_NodeDel(pHead,19);
	if (rv != 0)
	{
		printf("func SList_NodeDel() err:%d\n", rv);
		goto End;
	}
	rv = SList_Print(pHead);
	if (rv != 0)
	{
		printf("func SList_Print() err:%d\n", rv);
		goto End;
	}

End:
	rv = SList_Destory(pHead);
	if (rv != 0)
	{
		printf("func SList_Destory() err:%d\n", rv);
		goto End;
	}
	printf("func SList_Destory() ok\n");

	getchar();
	getchar();
	return 0;
}