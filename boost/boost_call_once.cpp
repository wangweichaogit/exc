/*
测试call_once,
boost::call_once 保证一个函数不被多个线程同时执行

init 只执行了一次
num 执行了5次
*/
#include <boost/thread/thread.hpp>
#include <boost/thread/once.hpp>
#include <cassert>
#include <iostream>
int value = 0;
int num = 0;
boost::once_flag once = BOOST_ONCE_INIT;
	
void init()
{
	++value;
}

void thread_proc()
{
	boost::call_once(&init,once);
	num++;
}

int main(int agrc,char *argv[])
{
	boost::thread_group threads;
		for (int i = 0; i < 5; i++)
		{
			threads.create_thread(&thread_proc);
		}
		threads.join_all();
		//assert(value == 1);
		std::cout<<"value = "<< value<<" ; num = "<<num<<std::endl;
		
}