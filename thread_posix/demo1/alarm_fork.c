#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc,char **argv)
{
	int status;
	char line[128];
	int seconds;
	pid_t pid;
	char message[64];
	while(1) {
		printf("alarm> :");
		if (fgets(line,sizeof(line),stdin) ==NULL)	
			exit(0);
		if (strlen(line)<= 1)
			continue;
		
		if (sscanf(line,"%d%s",&seconds,message) < 2)
			fprintf(stderr,"bad command\n");
		else
		{
			pid = fork();
			if (pid == -1)
				perror("fork");
			if (pid == 0)
			{
				sleep(seconds);
				printf("%dhou %s\n",seconds,message);
				exit(0);
			}
			else {
				do {
				pid = waitpid(-1,NULL,WNOHANG);
				if (pid == -1)
					perror("wait for child");
			}while(pid!=0);

		}
	}
}
}
