#ifndef __THREADPOOL_H_
#define __THREADPOOL_H_

//定义线程池结构体名称
typedef struct threadpool_t threadpool_t;

/*
函数功能        创建一个线程池
min_thr_num  	是线程维持的最小线程数目
max_thr_num  	是线程维持的最多线程数目
queue_max_size  工作队列的大小
返回值		成功返回值指向创建线程池的指针 失败返回NULL
*/
threadpool_t * threadpool_create(int min_thr_num,int max_thr_num,int queue_max_size);

/*
函数功能     从线程池取出一个线程，用于执行指定函数
第一个参数   创建出来的线程池指针
第二个参数   线程需要执行的函数
第三个参数   线程函数的参数
返回值       成功返回0 失败返回-1
*/
int threadpool_add(threadpool_t *pool,void *(*function)(void *arg),void *arg);

/*
函数功能   销毁线程池
返回值     成功返回0   失败返回-1
*/
int threadpool_destroy(threadpool_t * pool);

/*
函数功能    得到线程池内现有线程的数目
返回值	    成功返回数目  失败返回-1
*/
int threadpool_all_threadnum(threadpool_t *pool);

/*
函数功能    得到线程池中繁忙线程数目
返回值      成功返回数目   失败返回-1
*/
int threadpool_busy_threadnum(threadpool_t *pool);

#endif
