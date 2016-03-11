#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct msg{
struct msg *next;
int num;
};

struct msg *head;

void * do_prodect(void *argv)
{
	struct msg *new;
	while(1) {
	pthread_mutex_lock(&mutex);
	new = malloc(sizeof(struct msg));
	new->next=head;
	head=new;
	new->num=rand()%100+1;
	printf("prodect : %d\n",new->num);
	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond);
	sleep(rand()%5);
	}
}

void * do_consumer(void *arg)
{
	struct msg *new;
	while(1) {
	pthread_mutex_lock(&mutex);

	while(head == NULL)
		pthread_cond_wait(&cond,&mutex);


	new = head;
	head=head->next;
	printf("consumer: %d\n",new->num);
	free(new);
	pthread_mutex_unlock(&mutex);
	sleep(rand()%5);
	}
}


int main()
{
	pthread_t pid,pid2;
	srand(time(NULL));
	int status;
	status = pthread_create(&pid,NULL,do_prodect,NULL);
	if (status != 0)
		perror("pthread_create");
	status = pthread_create(&pid2,NULL,do_consumer,NULL);
	if (status != 0)
		perror("pthread_create");


	pthread_join(pid ,NULL);
	pthread_join(pid2 ,NULL);
	return 0;
}
