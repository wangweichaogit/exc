#include "log.h"
#include <boost/bind.hpp>
void thread_proc(void* p)
{
	char buf[10];
	sprintf(buf,"%d\n",*(char *)p);
	LogPrint(buf);
}
int main()
{
	
		boost::thread_group threads;
		char buf[50] = { 0 };
		for (int i = 0; i < 50; i++)
		{
			buf[i]= i;
			threads.create_thread(boost::bind(thread_proc,(void *)(buf+i)));
		}
		threads.join_all();

	return 0;
}