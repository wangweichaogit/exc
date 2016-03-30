#include "csem.hpp"
#include <iostream>

#define COUNT 2
static	CSemaphore *psem = NULL;


void openconnections1(void *arg)
{
	int i = 0;
	while(1) {
			sleep(1);
	CSemaphoreGrant grant(*psem);
	i++;
	std::cout<<"thread1: i = "<<i<<std::endl;
	

	}
}

void openconnections2(void *arg)
{
	int i = 0;
	while(1) {
			sleep(2);
	CSemaphoreGrant grant(*psem);
	i++;
	std::cout<<"thread2: i = "<<i<<std::endl;
	
	}
}
void openconnections3(void *arg)
{
	int i = 0;
	while(1) {
	sleep(3);
	CSemaphoreGrant grant(*psem);
	i++;
	std::cout<<"thread3: i = "<<i<<std::endl;


	}
}
void openconnections4(void *arg)
{
	int i = 0;
	while(1) {

	sleep(4);
	CSemaphoreGrant grant(*psem);
	i++;
	std::cout<<"thread4: i = "<<i<<std::endl;

	}
}
int main()
{
	psem = new CSemaphore(COUNT);
	
	boost::thread(openconnections1,(void *)NULL);
	boost::thread(openconnections2,(void *)NULL);
	boost::thread(openconnections3,(void *)NULL);
	boost::thread(openconnections4,(void *)NULL);
	sleep(6000);
	delete psem;
	return 0;
}