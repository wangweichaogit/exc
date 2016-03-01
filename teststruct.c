#include <stdio.h>
#include <stdlib.h>

struct message
{
	int length;
	char data[0];
};

int main()
{
	printf("size = %d\n",sizeof(message));
	message *mg = (message *)malloc(12);
	for (int i = 0; i < 8 ; i++)
	{
		mg->data[i] ='1';
	}
	printf("data = %s\n",mg->data);
	free(mg);
	return 0;
}
