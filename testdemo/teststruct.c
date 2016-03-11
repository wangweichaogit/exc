#include <stdio.h>
#include <stdlib.h>

struct message
{
	int length;
	char data[0];
};
//这两种用法区别是，第一个结构体只用分配一次连续的内存
//而第二个结构需要分配两次内存，切分配的内存是不连续的
struct message2
{
	int length;
	char *data;
};

int main()
{
	printf("size = %d\n",sizeof(message));
	int length = 10;
	message *mg = (message *)malloc(sizeof(message)+length);
	mg->length = length;
	for (int i = 0; i < mg->length ; i++)
	{
		mg->data[i] ='1';
	}
	printf("data = %s\n",mg->data);
	free(mg);
	return 0;
}
