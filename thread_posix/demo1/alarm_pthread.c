#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct alarm_tag {
	int seconds;
	char message[64];
}alarm_t;

void *alarm_thread(void *arg)
{
	alarm_t *alarm = (alarm_t *)arg;
	int status;
	status = pthread_detach(pthread_self());
	if (status != 0)
		perror("pthread_detach");
	sleep(alarm->seconds);
	printf("%d    %s\n",alarm->seconds,alarm->message);
	free(alarm);
	return NULL;
}
int main(int argc,char **argv)
{
	int status;
	char line[128];
	alarm_t * alarm;
	pthread_t pthread;
	while(2) {
		printf("alarm>: ");
		if (fgets(line,sizeof(line),stdin)== NULL)
			exit(0);
		if (strlen(line) <= 1)
			continue;
		alarm = malloc(sizeof(alarm_t));
		if (alarm == NULL)
			perror("malloc");
		if (sscanf(line,"%d%s",&(alarm->seconds),alarm->message) < 2) {
			fprintf(stderr,"bad command\n");
			free(alarm);
		}
		else {
			status = pthread_create(&pthread,NULL,alarm_thread,alarm);
			if (status != 0)
				perror("pthread_create");
		}
	
	}
	return 0;
}

