/*
g++ -g -o __thread __thread.cc Tls.cc -lpthread -Wall 

g++ -g -o __thread __thread.cc Tls.cc -lpthread -Wall -DUSE_TLS

valgrind -v --leak-check=full --tool=memcheck ./__thread
vargrind查看内存泄露

__thread:
1.__thread修饰一些类的线程局部变量时，往往容易造成内存泄漏

__thread可以修饰基本类型，string等类不能直接修饰，只能用于指针类型
__thread std::string thread_name; 错误
__thread std::string * thread_name;  正确使用
2.再多线程编程中，使用于global变量，是每个线程私有一份
*/
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <vector>
#include <string>

#ifdef USE_TLS
#include "Tls.h"
#endif
 
#define gettid() syscall(__NR_gettid)
using namespace std;
 
class TlsLeakTest
{
public:
    TlsLeakTest();
    ~TlsLeakTest();
    virtual void start();
    bool threadFun();
public:
    static void * threadEntry(void * arg);
#ifdef USE_TLS
    static void releaseTls(void * ptr);
#endif
protected:
    static __thread std::string *   _vecArray;
    uint32_t                    _threadNum;
    std::vector<pthread_t>        _vecThreadIds;
 
};
 
__thread string *   TlsLeakTest::_vecArray=NULL;
 
TlsLeakTest::TlsLeakTest()
{
    _threadNum=10;
}
TlsLeakTest::~TlsLeakTest()
{
}
void TlsLeakTest::start()
{
    pthread_t tid=0;
    for(uint32_t i=0; i < _threadNum; i++)
    {
         pthread_create(&tid, NULL, TlsLeakTest::threadEntry, this);
        _vecThreadIds.push_back(tid);
    }
    //waiting for threads to finish
    for(uint32_t i=0; i<_threadNum; i++)
    {
        if(_vecThreadIds[i] != 0)
        {
            pthread_join(_vecThreadIds[i], NULL);
            _vecThreadIds[i]=0;
        }
    }
}
bool TlsLeakTest::threadFun()
{
    char buff[128];
    int64_t tid=gettid();
    for(int32_t i=0; i < 100 ; i++)
    {
        if(NULL == _vecArray )
        {
            _vecArray=new string[100];
#ifdef USE_TLS
            Tls::pthread_atexit_add((void *)_vecArray,&TlsLeakTest::releaseTls);
            printf("register TlsLeakTest::releaseTls(): tid=%ld\n",gettid());
#endif
						
        }
        sprintf(buff,"%ld:%d",tid,i); 
        _vecArray[i]=buff;
        usleep(100000);
        if(99 == i)
        {
            printf("tid=%ld _vecArray's addr=%p _vecArray[99]=%s\n"
                                ,tid,_vecArray,_vecArray[99].c_str());
        }
      
    }
 		delete []_vecArray;
    return true;
 
}
 
void * TlsLeakTest::threadEntry(void * arg)
{
    TlsLeakTest *test=(TlsLeakTest*)arg;
    test->threadFun();
    return NULL;
}
 
#ifdef USE_TLS
void  TlsLeakTest::releaseTls(void * ptr)
{
    std::string * vecArray=(std::string *)ptr;
    if(vecArray)
    {
        delete []vecArray;
        vecArray=NULL;
        printf("TlsLeakTest::releaseTls(): tid=%ld\n",gettid());
    }
}
#endif
 
int main(int argc, char *argv[])
{
    TlsLeakTest test;
    test.start();
    return 0;
}
