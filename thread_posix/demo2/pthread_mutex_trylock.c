#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#define SPIN 10000000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
long counter;
time_t end_time;

void * counter_thread(void *arg)
{
	int status;
	int spin;
	
	while(time(NULL) < end_time)
	{
		status = pthread_mutex_lock(&mutex);
		if (status != 0)
			perror("pthread_mutex_lock");
		for (spin = 0; spin < SPIN ;spin++)
		{
			counter++;
		}
		status = pthread_mutex_unlock(&mutex);
		if (status != 0)
			perror("pthread_mutex_unlock");
		sleep(1);
	}
	printf("counter is %ld\n",counter);
	return NULL;
}


void *monitor_thread(void *arg)
{
	int status;
	int misses = 0;
	
	while(time(NULL) < end_time)
	{
		sleep(3);
		status = pthread_mutex_trylock(&mutex);
		
		if (status != EBUSY)
			{
				if (status != EBUSY)
					{
						if (status != 0)
							perror("trylock");
						printf("counter is %ld\n",counter);
						status = pthread_mutex_unlock(&mutex);
						if (status != 0)
							{
								perror("unlock");
							}
					}
					else
						misses++;
			}
	}
	printf("monitor thread missed %d times\n",misses);
	return NULL;
}

int main()
{
	int status;
	
	pthread_t counter_pthread_id,monitor_pthread_id;
	
	end_time = time(NULL) + 60;
	status = pthread_create(&counter_pthread_id,NULL,counter_thread,NULL);
	if (status != 0)
		perror("pthread_create");
		
	status = pthread_create(&monitor_pthread_id,NULL,monitor_thread,NULL);
	if (status != 0)
		perror("pthread_create");
		
	status = pthread_join(counter_pthread_id,NULL);
	if (status != 0)
		perror("pthread_join");
	status = pthread_join(monitor_pthread_id,NULL);
	if (status != 0)
		perror("pthread_join");	
	return 0;
}