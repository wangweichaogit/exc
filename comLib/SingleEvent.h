// SingleEvent.h: interface for the SingleEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLEEVENT_H__97DFDCA6_446A_403B_8563_EF4DCA7AD478__INCLUDED_)
#define AFX_SINGLEEVENT_H__97DFDCA6_446A_403B_8563_EF4DCA7AD478__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comLib/comdefine.h" 

namespace DHT
{

class SingleEvent  
{
public:
	bool Wait(int nTimeout);
	bool Wait();//wait till the event becomes signaled  Wait(INFINITE)
	bool Reset();
	bool Set();
	SingleEvent(bool bManualReset=false, bool bInitialState=false);
	virtual ~SingleEvent();
protected:

#ifdef WIN32
    HANDLE m_handleEvent;	//m_handleEvent=CreateEvent(NULL,true, false,NULL);
#else
	pthread_cond_t m_cond;
	pthread_mutex_t m_mutex;
	bool m_signalled;
	bool m_bManualReset;
#endif
};

#ifdef WIN32
inline SingleEvent::SingleEvent(bool bManualReset, bool bInitialState)
{
	m_handleEvent=CreateEvent(NULL,bManualReset, bInitialState,NULL);	
}
#else
inline SingleEvent::SingleEvent(bool bManualReset, bool bInitialState)
:m_signalled(bInitialState),m_bManualReset(bManualReset)
{
	pthread_mutex_init(&m_mutex, 0);
	pthread_cond_init(&m_cond, 0);
}
#endif


inline SingleEvent::~SingleEvent()
{
#ifdef WIN32
	CloseHandle(m_handleEvent);
#else
	pthread_mutex_destroy(&m_mutex);
	pthread_cond_destroy(&m_cond);
#endif
}

inline bool SingleEvent::Set()
{
#ifdef WIN32
	return SetEvent(m_handleEvent);
#else
    pthread_mutex_lock(&m_mutex);
	pthread_cond_signal(&m_cond);
	m_signalled = true;
    pthread_mutex_unlock(&m_mutex);
    return true;
#endif
}

inline bool SingleEvent::Reset()
{
#ifdef WIN32
	return ResetEvent(m_handleEvent);
#else
    pthread_mutex_lock(&m_mutex);
	m_signalled = false;
    pthread_mutex_unlock(&m_mutex);
    return true;
#endif
}

inline bool SingleEvent::Wait()
{
#ifdef WIN32
	return ( WAIT_OBJECT_0 == WaitForSingleObject(m_handleEvent,INFINITE));
#else
    bool bRet = false;
    pthread_mutex_lock(&m_mutex);
	if((true == m_bManualReset) && (true == m_signalled))
		bRet = true;
    else
        bRet = (0==pthread_cond_wait(&m_cond, &m_mutex));
    pthread_mutex_unlock(&m_mutex);

	return bRet;
#endif
}

inline bool SingleEvent::Wait(int nTimeout)
{
#ifdef WIN32
	return (WAIT_OBJECT_0 == WaitForSingleObject(m_handleEvent,nTimeout));
#else
    bool bRet = false;
    pthread_mutex_lock(&m_mutex);
	if((true == m_bManualReset) && (true == m_signalled))
		bRet = true;
    else
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        int timeoutSec = nTimeout/1000;
        int timeoutMilliSec = nTimeout%1000;
        ts.tv_sec += timeoutSec;
        int tsMilliSec = ts.tv_nsec/1000/1000; //nanosecond to microsecond to millisecond  
        if ( tsMilliSec + timeoutMilliSec < 1000 )
        {
            ts.tv_nsec = ts.tv_nsec + timeoutMilliSec*1000*1000;
        }
        else
        {
            ts.tv_sec += 1;
            ts.tv_nsec =  (tsMilliSec + timeoutMilliSec -1000)*1000*1000;		
        }	
        bRet = (0==pthread_cond_timedwait(&m_cond, &m_mutex, &ts));
    }
    pthread_mutex_unlock(&m_mutex);
    return bRet;
#endif
}

} //namespace VOIP
#endif // !defined(AFX_SINGLEEVENT_H__97DFDCA6_446A_403B_8563_EF4DCA7AD478__INCLUDED_)
