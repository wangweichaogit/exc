// Thread.h: interface for the Thread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREAD_H__90F38EB7_A5E1_454D_94C0_6A699EA58051__INCLUDED_)
#define AFX_THREAD_H__90F38EB7_A5E1_454D_94C0_6A699EA58051__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comLib/comdefine.h" 
#include "comLib/SafeQueue.h"

#ifdef _ANDROID_DHN_
#include "DhnSdkJni.h"
#endif

#ifndef INFINITE
#define INFINITE 0xFFFFFFFF
#endif

namespace DHT
{
class Thread  
{
public:
	struct ThreadParam
	{
		Thread* pBaseThread;
		void*       lpParam;
	}m_sThreadParam;

	Thread();
	virtual ~Thread();
	int GetThreadId();
	bool ThreadHasEnd();
	virtual bool Stop();
	virtual bool SynStart(void* lpParam = 0,int nWaitTime = INFINITE); //In millisecond
	virtual bool SynStop(int nWaitTime = INFINITE); //In millisecond
	bool Start(void* lpParam = 0 );
	static void* ThreadFunc(void* lpParam);
	static void msleep(int milliseconds);
protected:
	bool   m_bRunning;
	bool   m_bHasEnd;//Thread Terminate
	long   m_ThreadId;
	bool IsExit();
	virtual void run(void* lpParam) = 0;
	SafeQueue<int> m_start_lock_queue;
	SafeQueue<int> m_stop_lock_queue;
	Mutex m_mutex;
};


inline Thread::Thread():m_bRunning(false),m_bHasEnd(false),m_ThreadId(0)
{
}
inline Thread::~Thread()
{
}
inline bool Thread::Stop()
{
	m_bRunning = false;
	return true;
}

inline bool Thread::SynStart(void* lpParam,int nWaitTime)
{
	SingleMutex s(m_mutex);
	m_bRunning = false;
	m_start_lock_queue.Clear();
	bool bRet = Start(lpParam);
	if (bRet)
	{
		int v = 0;
		bRet = m_start_lock_queue.Pop(v,nWaitTime);
	}
	return bRet;
}

inline bool Thread::SynStop(int nWaitTime)
{
	SingleMutex s(m_mutex);
	m_stop_lock_queue.Clear();
	m_bRunning = false;
	int v = 0;
	m_stop_lock_queue.Pop(v,nWaitTime);
	return true;
}

inline bool Thread::IsExit()
{
	return ( m_bRunning != true  );
}

inline bool Thread::ThreadHasEnd()
{
	return m_bHasEnd;
}

inline int Thread::GetThreadId()
{
	return m_ThreadId;
}
inline void* Thread::ThreadFunc(void* lpParam)
{
	struct Thread::ThreadParam* pThreadParam = (struct Thread::ThreadParam*)lpParam;
	Thread* pThread = pThreadParam->pBaseThread;
	pThread->m_bRunning = true;
	pThread->m_start_lock_queue.Push(1);

#ifdef _ANDROID_DHN_
	dhnSetJenv ( dhnAttachJenv () );
#endif

	pThread->run(pThreadParam->lpParam);

#ifdef _ANDROID_DHN_
	 dhnDetachJenv ();
#endif

	pThread->m_bRunning = false;
	pThread->m_bHasEnd = true;
	pThread->m_stop_lock_queue.Push(1);
	return 0;
}


////////////////////////////////////////////////////////////////////////////
#ifdef WIN32

inline bool Thread::Start(void* lpParam)
{
	m_sThreadParam.pBaseThread = this;
	m_sThreadParam.lpParam = lpParam;
	DWORD ThreadID;
	HANDLE hThread = CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE)ThreadFunc, &m_sThreadParam, 0 , &ThreadID);
	m_ThreadId = (int)ThreadID;
	CloseHandle(hThread);
	return true;
}

inline void Thread::msleep(int milliseconds)
{
	Sleep(milliseconds);
}

////////////////////////////////////////////////////////////////////////////
#else //linux
////////////////////////////////////////////////////////////////////////////
inline bool Thread::Start(void* lpParam)
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);
#ifdef _AIX
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setstacksize(&attr, 10*1024);	//the default stack size is only 96KB!
#endif
	m_sThreadParam.pBaseThread = this;
	m_sThreadParam.lpParam = lpParam;
	pthread_t ThreadID;
	//BOOL bReturn = SetEvent(m_hRunEvent);
	pthread_create(&ThreadID, &attr,
			ThreadFunc, &m_sThreadParam );
	pthread_detach(ThreadID);//分离线程

	pthread_attr_destroy(&attr);

	m_ThreadId = (long)ThreadID;
	return true;
}

inline void Thread::msleep(int milliseconds)
{
#ifndef _AIX
    struct timeval tm;
    int sec = 0;
    
    sec=(int) milliseconds / 1000;
    if (sec > 0) {
        tm.tv_sec = sec;
        tm.tv_usec = 0;
    } else {
        tm.tv_sec = 0;
        tm.tv_usec = milliseconds * 1000; //延时毫秒
    }
    select(0,NULL,NULL,NULL,&tm);
#else
	usleep(milliseconds*1000);
#endif
}

#endif  // WIN32 or linux
////////////////////////////////////////////////////////////////////////////

} //namespace comLib

#endif // !defined(AFX_THREAD_H__90F38EB7_A5E1_454D_94C0_6A699EA58051__INCLUDED_)
