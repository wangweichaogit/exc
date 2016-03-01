#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
	int i;
	for (i = 0; i < 6 ; i++)
	{
	printf("father pid = %d\n",getpid());
	if (fork() == 0 )
		printf("sun pid = %d \n ",getpid())	;

	}
	return 0;
}
