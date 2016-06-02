/******************************************************************************
Module:  binlock.cpp
Notices: Copyright (c) 2000 Jeffrey Richter
******************************************************************************/

//#include "stdafx.h"
#include "GMRWLockImp.h"

///////////////////////////////////////////////////////////////////////////////
namespace DHT
{
GMJRRWLock::GMJRRWLock() 
#ifndef WIN32
:rd_cnt(0),wr_cnt(0)
#endif
{
#ifdef WIN32
	// Initially no readers want access, no writers want access, and 
	// no threads are accessing the resource
	m_nWaitingReaders = m_nWaitingWriters = m_nActive = 0;
	m_hsemReaders = CreateSemaphore(NULL, 0, MAXLONG, NULL);
	m_hsemWriters = CreateSemaphore(NULL, 0, MAXLONG, NULL);
	InitializeCriticalSection(&m_cs);
#else
	pthread_mutex_init(&cnt_mutex, NULL);
	pthread_cond_init(&rw_cond, NULL);
#endif
}


///////////////////////////////////////////////////////////////////////////////


GMJRRWLock::~GMJRRWLock() 
{

#ifdef WIN32

#ifdef _DEBUG
	// A SWMRG shouldn't be destroyed if any threads are using the resource
	if (m_nActive != 0)
		DebugBreak();
#endif

	m_nWaitingReaders = m_nWaitingWriters = m_nActive = 0;
	DeleteCriticalSection(&m_cs);
	CloseHandle(m_hsemReaders);
	CloseHandle(m_hsemWriters);
#else
	pthread_mutex_destroy(&cnt_mutex);
	pthread_cond_destroy(&rw_cond);
#endif
}


///////////////////////////////////////////////////////////////////////////////


void GMJRRWLock::readLock() 
{
#ifdef WIN32
	// Ensure exclusive access to the member variables
	EnterCriticalSection(&m_cs);

	// Are there writers waiting or is a writer writing?
	BOOL fResourceWritePending = (m_nWaitingWriters || (m_nActive < 0));

	if (fResourceWritePending) {

		// This reader must wait, increment the count of waiting readers
		m_nWaitingReaders++;
	} else {

		// This reader can read, increment the count of active readers
		m_nActive++;
	}

	// Allow other threads to attempt reading/writing
	LeaveCriticalSection(&m_cs);

	if (fResourceWritePending) {

		// This thread must wait
		WaitForSingleObject(m_hsemReaders, INFINITE);
	}
#else
	pthread_mutex_lock(&cnt_mutex);
	while (wr_cnt > 0  )
	{
		pthread_cond_wait(&rw_cond,&cnt_mutex);
	}
	rd_cnt++;
	pthread_mutex_unlock(&cnt_mutex);
#endif
}


///////////////////////////////////////////////////////////////////////////////


void GMJRRWLock::writeLock() 
{
#ifdef WIN32

	// Ensure exclusive access to the member variables
	EnterCriticalSection(&m_cs);

	// Are there any threads accessing the resource?
	BOOL fResourceOwned = (m_nActive != 0);

	if (fResourceOwned) {

		// This writer must wait, increment the count of waiting writers
		m_nWaitingWriters++;
	} else {

		// This writer can write, decrement the count of active writers
		m_nActive = -1;
	}

	// Allow other threads to attempt reading/writing
	LeaveCriticalSection(&m_cs);

	if (fResourceOwned) {

		// This thread must wait
		WaitForSingleObject(m_hsemWriters, INFINITE);
	}
#else
	pthread_mutex_lock(&cnt_mutex);
	while(rd_cnt + wr_cnt > 0)
	{
		pthread_cond_wait(&rw_cond,&cnt_mutex);
	}
	wr_cnt++;
	pthread_mutex_unlock(&cnt_mutex);
#endif
}

///////////////////////////////////////////////////////////////////////////////

void GMJRRWLock::unlock() 
{
#ifdef WIN32
	// Ensure exclusive access to the member variables
	EnterCriticalSection(&m_cs);

	if (m_nActive > 0) {

		// Readers have control so a reader must be done
		m_nActive--;
	} else {

		// Writers have control so a writer must be done
		m_nActive++;
	}

	HANDLE hsem = NULL;  // Assume no threads are waiting
	LONG lCount = 1;     // Assume only 1 waiter wakes; always true for writers

	if (m_nActive == 0) {

		// No thread has access, who should wake up?
		// NOTE: It is possible that readers could never get access
		//       if there are always writers wanting to write

		if (m_nWaitingWriters > 0) {

			// Writers are waiting and they take priority over readers
			m_nActive = -1;         // A writer will get access
			m_nWaitingWriters--;    // One less writer will be waiting
			hsem = m_hsemWriters;   // Writers wait on this semaphore
			// NOTE: The semaphore will release only 1 writer thread

		} else if (m_nWaitingReaders > 0) {

			// Readers are waiting and no writers are waiting
			m_nActive = m_nWaitingReaders;   // All readers will get access
			m_nWaitingReaders = 0;           // No readers will be waiting
			hsem = m_hsemReaders;            // Readers wait on this semaphore
			lCount = m_nActive;              // Semaphore releases all readers
		} else {

			// There are no threads waiting at all; no semaphore gets released
		}
	}

	// Allow other threads to attempt reading/writing
	LeaveCriticalSection(&m_cs);

	if (hsem != NULL) {

		// Some threads are to be released
		ReleaseSemaphore(hsem, lCount, NULL);
	}
#else
	pthread_mutex_lock(&cnt_mutex);
	if (wr_cnt > 0)
	{
		wr_cnt--;
		pthread_cond_broadcast(&rw_cond);
	}
	else if( rd_cnt > 0 )
	{
		rd_cnt--;
		if (0 == rd_cnt)
		{
			pthread_cond_signal(&rw_cond);
		}
	}
	pthread_mutex_unlock(&cnt_mutex);
#endif
}
#ifndef WIN32
void GMJRRWLock::UnWritelock()
{
	pthread_mutex_lock(&cnt_mutex);
	wr_cnt--;
	pthread_cond_broadcast(&rw_cond);
	pthread_mutex_unlock(&cnt_mutex);
}
void GMJRRWLock::UnReadlock()
{
	pthread_mutex_lock(&cnt_mutex);
	rd_cnt--;
	if (0 == rd_cnt)
	{
		pthread_cond_signal(&rw_cond);
	}
	pthread_mutex_unlock(&cnt_mutex);
}
#endif


//////////////////////////////// End of File //////////////////////////////////

//#ifdef WIN32
//
//#include <cassert>
//
//GMWinRWLock::PROCINITSRWLOCK GMWinRWLock::m_funInitRWLock = NULL;
//GMWinRWLock::PROCACQSRWLOCKSHARE GMWinRWLock::m_funReadLock = NULL;
//GMWinRWLock::PROCRELSRWLOCKSHARE GMWinRWLock::m_funUnReadLock = NULL;
//GMWinRWLock::PROCACQSRWLOCKEXC GMWinRWLock::m_funWriteLock = NULL;
//GMWinRWLock::PROCRELSRWLOCKEXC GMWinRWLock::m_funUnWriteLock = NULL;
//unsigned long long GMWinRWLock::s_bInit = 0;
//
//bool GMWinRWLock::InitFunPtr()
//{
//	do
//	{
//		HMODULE hLibModule = NULL;
//
//		hLibModule = GetModuleHandle( "Kernel32" );
//		if( hLibModule == NULL )break;
//
//		m_funInitRWLock = (PROCINITSRWLOCK)GetProcAddress(
//			hLibModule, "InitializeSRWLock" );
//		if( m_funInitRWLock == NULL ) break;
//
//		m_funReadLock = (PROCACQSRWLOCKSHARE)GetProcAddress(
//			hLibModule, "AcquireSRWLockShared" );
//		if( m_funReadLock == NULL ) break;
//
//		m_funUnReadLock = (PROCRELSRWLOCKSHARE)GetProcAddress(
//			hLibModule, "ReleaseSRWLockShared" );
//		if( m_funUnReadLock == NULL ) break;
//
//		m_funWriteLock = (PROCACQSRWLOCKEXC)GetProcAddress(
//			hLibModule, "AcquireSRWLockExclusive" );
//		if( m_funWriteLock == NULL ) break;
//
//		m_funUnWriteLock = (PROCRELSRWLOCKEXC)GetProcAddress(
//			hLibModule, "ReleaseSRWLockExclusive" );
//		if( m_funUnWriteLock == NULL ) break;
//
//		s_bInit = 0xbfcde14af5e9f587;
//
//		return true;
//
//	} while (false);
//
//	return false;
//	
//}
//bool GMWinRWLock::init()
//{
//	if(s_bInit != 0xbfcde14af5e9f587)
//	{
//		if(!InitFunPtr())
//			return false;
//	}
//
//	m_funInitRWLock( &m_rwLock );
//	return true;
//}
//
//void GMWinRWLock::readLock()
//{
//	assert( m_funReadLock != NULL );
//	m_funReadLock( &m_rwLock );
//}
//
//void GMWinRWLock::unReadLock()
//{
//	assert( m_funUnReadLock != NULL );
//	m_funUnReadLock( &m_rwLock );
//}
//
//void GMWinRWLock::writeLock()
//{
//	assert( m_funWriteLock != NULL );
//	m_funWriteLock( &m_rwLock );
//}
//
//void GMWinRWLock::unWriteLock()
//{
//	assert( m_funUnWriteLock != NULL );
//	m_funUnWriteLock( &m_rwLock );
//}
//
//#endif

}//namespace DHT
