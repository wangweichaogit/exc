#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
/*
	启动一个线程
 输入定时任务，加入到全局链表中，
 等待定时
 输出
*/

typedef struct alarm_tag {
	struct alarm_tag *link;
	int 	seconds;
	time_t  time;
	char  	message[64];
}alarm_t;

pthread_mutex_t alarm_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t alarm_cond = PTHREAD_COND_INITIALIZER;

alarm_t *alarm_list = NULL;
time_t current_alarm= 0;

/* 链表插入*/
void alarm_insert(alarm_t *alarm)
{
	int status;
	alarm_t  **last,*next;

	last = &alarm_list;
	next = *last;
	while (next != NULL) {
		if (next->time >= alarm->time) {
		alarm->link = next;
		*last = alarm;
		break;
		}
		last = &next->link;
		next = next->link;
	}
	if (next == NULL) {
		*last = alarm;
		alarm->link = NULL;
	}

	if (current_alarm == 0 || alarm->time < current_alarm) {
	current_alarm = alarm->time;
	status = pthread_cond_signal(&alarm_cond);
	if (status != 0)
		perror("pthread_cond_signal");

	}

}	

void *alarm_thread(void *arg)
{
	alarm_t *alarm;
	struct timespec cond_time;
	time_t now;
	int status,expired;

	status = pthread_mutex_lock(&alarm_mutex);
	if (status != 0)
		perror("pthread_mutex_lock");

	while(1) {
	current_alarm = 0;
	while(alarm_list == NULL) {
		status = pthread_cond_wait(&alarm_cond,&alarm_mutex);
		if (status != 0)
			perror("wait on cond");

	}
	alarm = alarm_list;
	alarm_list = alarm->link;
	now = time(NULL);
	expired = 0;
	if (alarm->time > now ) {
		cond_time.tv_sec = alarm->time;
		cond_time.tv_nsec = 0;
		current_alarm = alarm->time;
		while(current_alarm == alarm->time) {
		status = pthread_cond_timedwait(&alarm_cond,&alarm_mutex,&cond_time);
		if (status == ETIMEDOUT)
			{
				expired = 1; break;
			}
		if (status != 0)
			printf("cond timedwait");
		}
		if (!expired)
			alarm_insert(alarm);
	}
	else
		expired = 1;
	if (expired) {
		printf("%d %s\n",alarm->seconds,alarm->message);
		free(alarm);
	}

	}
}	

int main(int argc,char **argv)
{
	int status;
	char line[128];
	alarm_t *alarm;
	pthread_t thread;
	
	status = pthread_create(&thread,NULL,alarm_thread,NULL);
	if (status != 0)
		perror("pthread_create");
	
	while(1) {
		printf("alarm > ");
		if (fgets(line,sizeof(line),stdin) == NULL)
			exit(0);
		if (strlen(line) <= 1)	
			continue;
		alarm = (alarm_t *)malloc(sizeof(alarm_t));
		if (alarm == NULL)
			perror("malloc");
		
		if (sscanf (line,"%d %s",&alarm->seconds,alarm->message) < 2) {
			fprintf(stderr,"bad command\n");
			free(alarm);
		} else {
			status = pthread_mutex_lock(&alarm_mutex);
			if (status != 0)
				perror("pthread_mutex_lock");
			alarm->time = time(NULL) + alarm->seconds;
			alarm_insert(alarm);
			status = pthread_mutex_unlock(&alarm_mutex);
			if (status != 0)
				perror("pthread_mutex_unlock");
		}
		
	}
}
