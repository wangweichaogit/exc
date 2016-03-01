#include <iostream>
#include <boost/thread.hpp>

#include <stdio.h>

using namespace std;
using namespace boost;
int i;
mutex mute;


class mutexlock
{
	private:
		unique_lock<mutex> lock;
	public:
	mutexlock(mutex &mutex):lock(mutex,defer_lock)
	{
		lock.lock();
	}
	~mutexlock()
	{
	//	lock.unlock();
	}
};

#define LOCK(s) mutexlock lock(s);
bool NewThread(void(*pfn)(void*), void* parg)
{
    try
    {
        boost::thread(pfn, parg); // thread detaches when out of scope
    } catch(boost::thread_resource_error &e) {
        printf("Error creating thread: %s\n", e.what());
        return false;
    }
    return true;
}

void count1(void *arg)
{
	while(true)
	{
		mute.lock();
		i++;
		cout<<"count1  :"<<i<<endl;
		mute.unlock();
		sleep(1);
		
	}
}

void count2(void *arg)
{
	while(true)
	{
		mute.lock();
		i++;
		cout<<"count2  :"<<i<<endl;
		mute.unlock();
		sleep(2);
		
	}
}


void count3(void *arg)
{
	while(true)
	{
		{unique_lock<mutex>  lock(mutex);
		i++;
	cout<<"count3  :"<<i<<endl;}
		sleep(2);	
	}
}
void count4(void *arg)
{
	while(true)
	{
		{unique_lock<mutex>  lock(mutex);
		i++;
	cout<<"count4  :"<<i<<endl;}
		sleep(1);	
	}
}

void count5(void *arg)
{
	while(true)
	{
		{LOCK(mute);
		i++;
	cout<<"count5  :"<<i<<endl;}
		sleep(1);	
	}
}


void count6(void *arg)
{
	while(true)
	{
		{LOCK(mute);
		i++;
	cout<<"count6  :"<<i<<endl;}
		sleep(1);	
	}
}


void test1()
{
	NewThread(count1,NULL);
	NewThread(count2,NULL);


}

void test2()
{
	NewThread(count3,NULL);
	NewThread(count4,NULL);
}


void test3()
{
	NewThread(count5,NULL);
	NewThread(count6,NULL);
}
int main()
{
	test3();
	while(1);
	return 0;
}
