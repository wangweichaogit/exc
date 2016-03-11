#include <stdio.h>
#include <pthread.h>
#include <errno.h>

typedef struct my_struct_tag {
	pthread_mutex_t  mutex;
	pthread_cond_t   cond;
	int              value;
}my_struct_t;

my_struct_t data = {
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_COND_INITIALIZER,
	0
	};

int hibernation = 1;

void * wait_thread(void *arg)
{
	int status;
	sleep(hibernation);
	status = pthread_mutex_lock(&data.mutex);
	if (status != 0)
		perror("pthread_mutex_lock");
	
	data.value = 1;
	status = pthread_cond_signal(&data.cond);
	if (status != 0)
		perror("pthread_cond_lock");

	status = pthread_mutex_unlock(&data.mutex);
	if (status != 0)
		perror("pthread_mutex_unlock");
	return NULL;
}


int main(int argc,char **argv)
{
	int status;
	pthread_t wait_thread_id;
	struct timespec timeout;	
	
	if (argc > 1)	
		hibernation = atoi(argv[1]);

	status = pthread_create(&wait_thread_id,NULL,wait_thread,NULL);
	if (status != 0)
		perror("pthread_create");

	timeout.tv_sec = time(NULL)+2;
	timeout.tv_nsec = 0;
	status = pthread_mutex_lock(&data.mutex);
	if (status != 0)
		perror("pthread_mutex_lock");
	
	while(data.value == 0) {
		status = pthread_cond_timedwait(&data.cond,&data.mutex,&timeout);
		if (status == ETIMEDOUT) {
			printf("condition wait timed out\n");
			break;
		}
		else if (status != 0)
			perror("pthread_cond_timedout");

	}
	
	if (data.value != 0)
		printf("condition was signaled\n");
	status = pthread_mutex_unlock(&data.mutex);
	return 0;
	
}
