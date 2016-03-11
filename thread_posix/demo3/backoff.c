/*
线程1 按1 2 3 加锁，trylock失败，将锁全部释放
线程2 按3 2 1 加锁


*/
#include <pthread.h>
#include <errno.h>
#include <stdio.h>

#define ITERATIONS 30


pthread_mutex_t mutex[3] = {
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER
};

int backoff = 1;
int yield_flag = 0;
void * lock_forward(void *arg)
{
	int i,iterate,backoffs;
	int status;
	for (iterate = 0; iterate < ITERATIONS;iterate++) {
		backoffs = 0;
		for (i = 0 ; i < 3; i++)
		{
			if (i == 0)
				{
					status = pthread_mutex_lock(&mutex[i]);
					if (status != 0)
						perror("pthread_mutex_lock");
					printf("lock_forward lock got %d\n",i);
				}
			else {
				if(backoff)
					status = pthread_mutex_trylock(&mutex[i]);
				else
					status = pthread_mutex_lock(&mutex[i]);
				if (status == EBUSY) {
					backoffs++;
					printf("lock_forward lock back off at %d\n",i);
					for (; i >= 0; i--) {
						status = pthread_mutex_unlock(&mutex[i]);
						if (status != 0)
							printf("backoff\n");
					}
				} else  {
					if (status != 0)
						printf("lock mutex\n");
						printf("lock_forward lock got %d\n",i);
				}
			}
		
		if (yield_flag) {
			if (yield_flag > 0)
				sched_yield();
			else
				sleep(1);
			}
	}
	printf("lock_forward lock backward got all locks, %d backoffs\n",backoffs);
	pthread_mutex_unlock(&mutex[0]);
	pthread_mutex_unlock(&mutex[1]);
	pthread_mutex_unlock(&mutex[2]);
	sched_yield();
	}
	return NULL;
}

void * lock_backward(void *arg)
{
	int i,iterate,backoffs;
	int status;
	for (iterate = 0; iterate < ITERATIONS;iterate++) {
		backoffs = 0;
		for (i = 2 ; i >= 0; i--)
		{
			if (i == 2)
				{
					status = pthread_mutex_lock(&mutex[i]);
					if (status != 0)
						perror("first lock");
					printf("lock_backward backward lock got %d\n",i);
				}
			else {
				if(backoff)
					status = pthread_mutex_trylock(&mutex[i]);
				else
					status = pthread_mutex_lock(&mutex[i]);
				if (status == EBUSY) {
					backoffs++;
					printf("lock_backward lock back off at %d\n",i);
					for (; i < 3; i++) {
						status = pthread_mutex_unlock(&mutex[i]);
						if (status != 0)
							printf("backoff\n");
					}
				} else  {
					if (status != 0)
						printf("lock mutex\n");
						printf("lock_backward backward lock got %d\n",i);
				}
			}
		
		if (yield_flag) {
			if (yield_flag > 0)
				sched_yield();
			else
				sleep(1);
			}
	}
	printf("lock_backward lock backward got all locks, %d backoffs\n",backoffs);
	pthread_mutex_unlock(&mutex[0]);
	pthread_mutex_unlock(&mutex[1]);
	pthread_mutex_unlock(&mutex[2]);
		sched_yield();
	}
	return NULL;
}

int main()
{
	pthread_t forward,backward;
	int status;
	status = pthread_create(&forward,NULL,lock_forward,NULL);
	status = pthread_create(&backward,NULL,lock_backward,NULL);
	pthread_exit(NULL);
	return 0;
}