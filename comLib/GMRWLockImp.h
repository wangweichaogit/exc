#ifndef _RWLOCKIMP_H_2008_GM_HQY
#define _RWLOCKIMP_H_2008_GM_HQY

#ifdef WIN32
#include <windows.h>
#else // linux
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;
#endif

#include "comLib/Stdafx.h"
//#include "GMlockimp.h"
/******************************************************************************
Module:  binlock.H
Notices: Copyright (c) 2006 ,引用《WIN32核心编程》中第十章关于读写锁的示例
******************************************************************************/
namespace DHT
{

//#ifdef WIN32
//
//class GMWinRWLock
//{
//
//
//	typedef struct GMSRWLOCK
//	{
//		PVOID Ptr;
//	} GMSRWLOCK, *PGMSRWLOCK;
//
//	typedef VOID (WINAPI *PROCINITSRWLOCK)(PGMSRWLOCK);
//	typedef VOID (WINAPI *PROCACQSRWLOCKSHARE)(PGMSRWLOCK);
//	typedef VOID (WINAPI *PROCRELSRWLOCKSHARE)(PGMSRWLOCK);
//	typedef VOID (WINAPI *PROCACQSRWLOCKEXC)(PGMSRWLOCK);
//	typedef VOID (WINAPI *PROCRELSRWLOCKEXC)(PGMSRWLOCK);
//
//	static PROCINITSRWLOCK m_funInitRWLock;
//	static PROCACQSRWLOCKSHARE m_funReadLock;
//	static PROCRELSRWLOCKSHARE m_funUnReadLock;
//	static PROCACQSRWLOCKEXC m_funWriteLock;
//	static PROCRELSRWLOCKEXC m_funUnWriteLock;
//
//	static unsigned long long s_bInit;
//
//	//static PROCINITSRWLOCK m_funInitRWLock;
//	//static PROCACQSRWLOCKSHARE m_funReadLock;
//	//static PROCRELSRWLOCKSHARE m_funUnReadLock;
//	//static PROCACQSRWLOCKEXC m_funWriteLock;
//	//static PROCRELSRWLOCKEXC m_funUnWriteLock;
//
//	/*static bool m_bInitFunAddress;
//	static GMCSLock m_bInitFunAddressLock;*/
//
//	GMSRWLOCK m_rwLock;
//	//static GMLock s_lock;
//	static bool InitFunPtr();
//public:
//
//	GMWinRWLock()
//	{
//		memset(this,0,sizeof(GMWinRWLock));
//
//	}
//	bool init();
//	void readLock();
//	void writeLock();
//	void unReadLock();
//	void unWriteLock();
//};
//
//#endif


class COMLIBAPI GMJRRWLock
{
public:
	GMJRRWLock();                 // Constructor
	~GMJRRWLock();                // Destructor

	void readLock();        // Call this to gain shared read access
	void writeLock();       // Call this to gain exclusive write access
	void unlock();			// Call this when done accessing the resource
#ifndef WIN32
	void UnReadlock();
	void UnWritelock();
#endif

private:
#ifdef WIN32
	CRITICAL_SECTION m_cs;    // Permits exclusive access to other members
	HANDLE m_hsemReaders;     // Readers wait on this if a writer has access
	HANDLE m_hsemWriters;     // Writers wait on this if a reader has access
	int    m_nWaitingReaders; // Number of readers waiting for access
	int    m_nWaitingWriters; // Number of writers waiting for access
	int    m_nActive;         // Number of threads currently with access
	//   (0=no threads, >0=# of readers, -1=1 writer)
#else
	pthread_mutex_t cnt_mutex;
	pthread_cond_t rw_cond;
	int rd_cnt, wr_cnt;
#endif
};

}//namespace
/*
成员 描述 
m _ c s  用于保护所有的其他成员变量，这样，对它们的操作就能够以原子操作方式来完成 
m _ n A c t i v e 用于反映共享资源的当前状态。如果该值是0，那么没有线程在访问资源。如果该值大于0，这个值用于表示当前读取该资源的线程的数量。如果这个数量是负值，那么写入程序正在将数据写入该资源。唯一有效的负值是- 1 
m _ n Wa i t i n g R e a d e r s 表示想要访问资源的阅读线程的数量。该值被初始化为0，当m _ n A c t i v e是- 1时，每当线程调用一次Wa i t To R e a d，该值就递增1 
m _ n Wa i t i n g Wr i t e r s 表示想要访问资源的写入线程的数量。该值被初始化为0，当m _ n A c t i v e大于0时，每当线程调用一次Wa i t To Wr i t e，该值就递增1 
m _ h s e m Wr i t e r s 当线程调用Wa i t To Wr i t e，但是由于m _ n A c t i v e大于0而被拒绝访问时，所有写入线程均等待该信标。当一个线程正在等待时，新阅读线程将被拒绝访问该资源。这可以防止阅读线程垄断该资源。当最后一个拥有资源访问权的阅读线程调用D o n e时，该信标就被释放，其数量是1，从而唤醒一个正在等待的写入线程 
m _ h s e m R e a d e r s 当许多线程调用Wa i t To R e a d，但是由于m _ n A c t i v e是- 1而被拒绝访问时，所有阅读线程均等待该信标。当最后一个正在等待的阅读线程调用D o n e时，该信标被释放，其数量是m _ n Wa i t i n g R e a d e r s，从而唤醒所有正在等待的阅读线程 
*/

#endif