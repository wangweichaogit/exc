
#include "stdio.h"
#include "string.h"
#include "malloc.h"







struct node {
	int data;
	char name[24];
	char age ;
	struct node *next;
};
typedef struct node SLIST;


SLIST * SList_Creat_Stack()
{
	SLIST node1, node2, node3;
	SLIST *pCur = NULL;
	
	//输出结点
	memset(&node1, 0, sizeof(SLIST));
	node1.data = 10;
	memset(&node2, 0, sizeof(SLIST));
	node2.data = 20;
	memset(&node3, 0, sizeof(SLIST));
	node3.data = 30;
	//建立链表
	
	node1.next = &node2;
	node2.next = &node3;
	node3.next = NULL;

	//打印输出
	pCur = &node1; //静态链表
	while (pCur) 
	{
		printf("data:%d \t", pCur->data);
		pCur = pCur->next;
	}
	
	pCur = &node1;
	return pCur;
}

int main()
{

	SLIST *pHead = NULL, *pCur = NULL;
	//创建静态链表
	pHead = SList_Creat_Stack();
	
	//遍历链表
	pCur = pHead;
	while (pCur) 
	{
		printf("data:%d \t", pCur->data);
		pCur = pCur->next;
	}

}
