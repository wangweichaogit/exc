/*
每个线程最多只能有一个perthread对象的demo
*/
#include <iostream>
#include <sys/types.h>
#include <pthread.h>
#include <assert.h>
class perthread;
__thread perthread* onethread = NULL;
class perthread
{
	public:
		perthread():tid(pthread_self()){
			if (!onethread){
				onethread = this;
				std::cout<<"creat one object tid = "<<tid<<std::endl;
			}
			else {
				std::cout<<"have pthread in tid = "<<tid<<std::endl;
				assert(onethread == NULL);
				}
		};
		~perthread()
		{
			tid = 0;
			onethread = NULL;
		}
	private:
		pthread_t tid;
};

void *testfunc(void *arg)
{
	{
		perthread p1;
	}
		perthread p2;
		#ifdef WRONG
		perthread p3;
		#endif
		return NULL;
}
int main()
{
	{
	perthread p1;
}
	pthread_t pid;
	pthread_create(&pid,NULL,testfunc,NULL);
	pthread_join(pid,NULL);
	
	perthread p2;

	#ifdef WRONG
	perthread p3;
	#endif
	return 0;
}