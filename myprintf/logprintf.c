#include <stdarg.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>

int logprintf(const char *format,...)
{
	int ret = 0;
	static FILE *fp = NULL;
	if (!fp)
	{
		const char *path = "./print.log";
		fp = fopen(path,"a");
		if (fp)
			setbuf(fp,NULL);
	}
	if (fp)
	{
//		static pthread_mutex_t mutex;
//		pthread_mutex_init(&mutex,NULL);
//		pthread_mutex_lock(&mutex);
		va_list arg_ptr;
		va_start(arg_ptr,format);
		ret = vfprintf(fp,format,arg_ptr);
		va_end(arg_ptr);
//		pthread_mutex_unlock(&mutex);
	}
	return 0;
}
void* function1(void *arg)
{
	srandom(time(NULL));	
	while(1)
	{
	time_t t =time(NULL);
	logprintf("%d: %s  ===%s\n",pthread_self(),"1111222233334444555566667777",ctime(&t));
	sleep(random()%2);
	}
	return NULL;
}
int main()
{
	pthread_t pt1,pt2,pt3,pt4,pt5,pt6;
	pthread_create(&pt1,NULL,function1,NULL);
	pthread_create(&pt2,NULL,function1,NULL);
	pthread_create(&pt3,NULL,function1,NULL);
	pthread_create(&pt4,NULL,function1,NULL);
	pthread_create(&pt5,NULL,function1,NULL);
	pthread_create(&pt6,NULL,function1,NULL);
	pthread_join(pt1,NULL);
	pthread_join(pt2,NULL);
	
	return 0;
}
