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
Notices: Copyright (c) 2006 ,���á�WIN32���ı�̡��е�ʮ�¹��ڶ�д����ʾ��
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
��Ա ���� 
m _ c s  ���ڱ������е�������Ա�����������������ǵĲ������ܹ���ԭ�Ӳ�����ʽ����� 
m _ n A c t i v e ���ڷ�ӳ������Դ�ĵ�ǰ״̬�������ֵ��0����ôû���߳��ڷ�����Դ�������ֵ����0�����ֵ���ڱ�ʾ��ǰ��ȡ����Դ���̵߳������������������Ǹ�ֵ����ôд��������ڽ�����д�����Դ��Ψһ��Ч�ĸ�ֵ��- 1 
m _ n Wa i t i n g R e a d e r s ��ʾ��Ҫ������Դ���Ķ��̵߳���������ֵ����ʼ��Ϊ0����m _ n A c t i v e��- 1ʱ��ÿ���̵߳���һ��Wa i t To R e a d����ֵ�͵���1 
m _ n Wa i t i n g Wr i t e r s ��ʾ��Ҫ������Դ��д���̵߳���������ֵ����ʼ��Ϊ0����m _ n A c t i v e����0ʱ��ÿ���̵߳���һ��Wa i t To Wr i t e����ֵ�͵���1 
m _ h s e m Wr i t e r s ���̵߳���Wa i t To Wr i t e����������m _ n A c t i v e����0�����ܾ�����ʱ������д���߳̾��ȴ����űꡣ��һ���߳����ڵȴ�ʱ�����Ķ��߳̽����ܾ����ʸ���Դ������Է�ֹ�Ķ��߳�¢�ϸ���Դ�������һ��ӵ����Դ����Ȩ���Ķ��̵߳���D o n eʱ�����ű�ͱ��ͷţ���������1���Ӷ�����һ�����ڵȴ���д���߳� 
m _ h s e m R e a d e r s ������̵߳���Wa i t To R e a d����������m _ n A c t i v e��- 1�����ܾ�����ʱ�������Ķ��߳̾��ȴ����űꡣ�����һ�����ڵȴ����Ķ��̵߳���D o n eʱ�����ű걻�ͷţ���������m _ n Wa i t i n g R e a d e r s���Ӷ������������ڵȴ����Ķ��߳� 
*/

#endif