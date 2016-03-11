#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(int argc,char **argv)
{
	int seconds;
	char line[28];
	char message[64];
	
	while(1){
	printf("alarm> ");
	if (fgets(line,sizeof(line),stdin)== NULL)
		exit(0);
	if (strlen(line) <= 1)
		continue;
	if (sscanf(line,"%d%s",&seconds,message) < 2)
		fprintf(stderr,"bad command\n");
	else
	{
	sleep(seconds);
	printf("%ds %s\n",seconds,message);
	}
}
	return 0;
}
