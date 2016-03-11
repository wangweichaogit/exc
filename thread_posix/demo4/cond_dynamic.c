#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

typedef struct my_struct_tag {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int 		value;
}my_struct_t;

int main(int argc,char **argv)
{
	my_struct_t *data;
	int status;
	data = malloc(sizeof(my_struct_t));
	if (data == NULL)
		perror("malloc");

	status = pthread_mutex_init(&data->mutex,NULL);
	if (status != 0)
		perror("pthread_mutex_init");
	status = pthread_cond_init(&data->cond,NULL);
	if (status != 0)
		perror("pthread_cond_init");
	

	pthread_cond_destroy(&data->cond);
	pthread_mutex_destroy(&data->mutex);
	free(data);
	return status;
}
