/*
流水线线程模型
*/
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>

/*
stage_t  流水线中传递的变量类型
avail    通知某步要操作的数据准备好了
ready   准备处理新数据时发信号给ready
data    是上个流水传下来的数据
data_ready   标志位，1为数据准备好 0为没有数据
*/
typedef struct stage_tag {
	pthread_mutex_t mutex;
	pthread_cond_t avail;
	pthread_cond_t ready;
	int		data_ready;
	long		data;
	pthread_t 	thread;
	struct stage_tag	*next;
}stage_t;

typedef struct pipe_tag {
	pthread_mutex_t 	mutex;
	stage_t 		*head;
	stage_t			*tail;
	int			stages;
	int 			active;
}pipe_t;

/*
	将数据传入到 数据变量中
	
*/
int pipe_send(stage_t *stage,long data)
{
	int status;
	status = pthread_mutex_lock(&stage->mutex);
	if (status != 0)
		return status;

	//发送给入口时 不阻塞；
	#if 0
	while (stage->data_ready) {
		status = pthread_cond_wait(&stage->ready,&stage->mutex);
		if (status != 0) {
			pthread_mutex_unlock(&stage->mutex);
			return status;
		}
	}	
	#endif
		stage->data = data;
		stage->data_ready = 1;
		status = pthread_cond_signal(&stage->avail);
		if (status != 0) {
			pthread_mutex_unlock(&stage->mutex);
			return status;
		}
		status = pthread_mutex_unlock(&stage->mutex);
		return status;
		
}

/*线程函数，阻塞等待数据到来
	数据到来之后,值+1，传入下个数据变量中
	发送信号 激活下个线程
*/
void *pipe_stage(void *arg)
{
	stage_t *stage = (stage_t *)arg;
	stage_t *next_stage = stage->next;
	int status;
	status = pthread_mutex_lock(&stage->mutex);
	if (status != 0)
		perror("pthread_mutex_lock");
	
	while(1) {	
		while (stage->data_ready != 1) {
			status = pthread_cond_wait(&stage->avail,&stage->mutex);
			if (status != 0)
				perror("pthread_cond_wait");
		}
		pipe_send(next_stage,stage->data + 1);
		stage->data_ready = 0;
		#if 0
		status = pthread_cond_signal(&next_stage->ready);
		#endif
		if (status != 0)
			perror("pthread_cond_signal");
	}
}

//创建stages个线程，每个线程执行pipe_stage函数
int pipe_create(pipe_t *pipe,int stages)
{
	int pipe_index;
	stage_t **link = &pipe->head;
	stage_t *new_stage,*stage;
	int status;
		
	status = pthread_mutex_init(&pipe->mutex,NULL);
	if (status != 0)
		perror("pthread_mutex_init");
	pipe->stages = stages;
	pipe->active = 0;
	for (pipe_index = 0; pipe_index <= stages; pipe_index++) {
		new_stage = (stage_t *)malloc(sizeof(stage_t));
		status = pthread_mutex_init(&new_stage->mutex,NULL);
		status = pthread_cond_init(&new_stage->avail,NULL);
		status = pthread_cond_init(&new_stage->ready,NULL);
		new_stage->data_ready = 0;
		*link = new_stage;
		link = &new_stage->next;
	}
	*link = (stage_t *)NULL;
	pipe->tail = new_stage;
	for (stage = pipe->head; stage->next != NULL; stage = stage->next) {
		status = pthread_create(&stage->thread,NULL,pipe_stage,(void * )stage);
	}
	return 0;
}

//程序开始  输入数据传入到第一个结构中去
int pipe_start(pipe_t *pipe,long value)
{
	int status;
	status = pthread_mutex_lock(&pipe->mutex);
	pipe->active++;
	status = pthread_mutex_unlock(&pipe->mutex);
	pipe_send(pipe->head,value);
	return 0;
}

//从结尾获取结果
int pipe_result(pipe_t *pipe,long *result)
{
	stage_t *tail = pipe->tail;
	long value;
	int empty = 0;
	int status ;
	status = pthread_mutex_lock(&pipe->mutex);
	if (pipe->active <= 0)
		empty = 1;
	else 
		pipe->active--;
	status = pthread_mutex_unlock(&pipe->mutex);
	if (empty)
		return 0;


	pthread_mutex_lock(&tail->mutex);
	while(!tail->data_ready)
		pthread_cond_wait(&tail->avail,&tail->mutex);
	*result = tail->data;
	tail->data_ready = 0;
	pthread_cond_signal(&tail->ready);
	pthread_mutex_unlock(&tail->mutex);
	return 1;
}


int main(int argc,char **argv)
{
	pipe_t my_pipe;
	long value,result;
	int status;
	char line[128];
	pipe_create(&my_pipe,10);
	printf("enter int value\n");

	while(1) {
		printf("data>");
		if (fgets(line,sizeof(line),stdin) == NULL)
			return 0;
		if (strlen(line) <= 1)
			continue;
		if (strlen(line) <= 2 && line[0] == '=' ) {
			if (pipe_result(&my_pipe,&result))
				printf("result is %ld \n",result);
			else
				printf("pipe is empty\n");
		}
		else {
			if (sscanf(line,"%ld",&value) < 1)
				printf("enter an integer value\n");
			else
				pipe_start(&my_pipe,value);
		}
	}
}
