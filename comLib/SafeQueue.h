// SafeQueue.h: interface for the SafeQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAFEQUEUE_H__9EB09358_A23A_41C2_899C_B73E9AC2632C__INCLUDED_)
#define AFX_SAFEQUEUE_H__9EB09358_A23A_41C2_899C_B73E9AC2632C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <queue>

#include "comLib/comdefine.h" 
#include "comLib/Mutex.h"
#include "comLib/Semaphore.h"

#ifndef SEM_VALUE_MAX
#define SEM_VALUE_MAX  INT_MAX
#endif


namespace DHT
{
//------------Block SafeQueue----------------
template<class T> class SafeQueue 
{
	enum{MAX_DEFAULT_SIZE=16*1024, MAX_QUEUE_SIZE=4000*1024};
public:
	SafeQueue(size_t queue_size = MAX_DEFAULT_SIZE);
	virtual ~SafeQueue();
	bool Push(const T& value);
	void BlockPush(const T& value);
	bool BlockPop(T& ret_value);
	bool Pop(T& ret_value, int timeout);
	size_t GetSize();
	bool IsEmpty();
	void Clear();
protected:
	std::queue<T> m_Queue;
	Mutex m_Lock;
	size_t m_queue_size;
	Semaphore m_EmptySemaphore;
	Semaphore m_FullSemaphore;
};


//////////////////////////////////////////////////
//------------Block SafeQueue----------------
//////////////////////////////////////////////////////////////////////
template<class T>
SafeQueue<T>::SafeQueue(size_t queue_size) :
#ifdef WIN32
m_queue_size(queue_size > MAX_QUEUE_SIZE ? MAX_QUEUE_SIZE : queue_size), 
#else
m_queue_size(queue_size > SEM_VALUE_MAX ? SEM_VALUE_MAX : queue_size), 
#endif
m_EmptySemaphore(m_queue_size,m_queue_size),
m_FullSemaphore(0, m_queue_size)
{
}
template<class T>
SafeQueue<T>::~SafeQueue() 
{
}

template<class T>
bool SafeQueue<T>::Push(const T& value)
{
	SingleMutex s(m_Lock);
	if(m_Queue.size() >= m_queue_size)//maybe overflow, need log out
	{
		//change by zw.
		//m_Queue.pop();	
		//m_FullSemaphore.Wait();
		//m_EmptySemaphore.Post();

		return false;
	}

	m_EmptySemaphore.Wait();//等待空位置
	m_Queue.push(value);
	m_FullSemaphore.Post();	//+1

	return true;
}

template<class T>
void SafeQueue<T>::BlockPush(const T& value)
{
	m_EmptySemaphore.Wait();//等待空位置
	{
		SingleMutex s(m_Lock);
		//m_Lock.Lock();
		m_Queue.push(value);
		m_FullSemaphore.Post();	//+1
		//m_Lock.Unlock();
	}
}

template<class T>
bool SafeQueue<T>::BlockPop(T& ret_value)
{
	m_FullSemaphore.Wait();	//-1
	SingleMutex s(m_Lock);
	//if(m_Queue.empty())
	//	return false;
	ret_value = m_Queue.front();
	m_Queue.pop();
	m_EmptySemaphore.Post();
	return true;
}
template<class T>
bool SafeQueue<T>::Pop(T& ret_value, int timeout)
{
	if (true == m_FullSemaphore.Wait(timeout))// have  -1
	{
		SingleMutex s(m_Lock);
		//m_Lock.Lock();
		ret_value = m_Queue.front();
		m_Queue.pop();
		m_EmptySemaphore.Post();
		//m_Lock.Unlock();
		return true;
	}
	return false;
}

template<class T>
size_t SafeQueue<T>::GetSize()
{
	SingleMutex s(m_Lock);
	return m_Queue.size();
}

template<class T>
bool SafeQueue<T>::IsEmpty()
{
	SingleMutex s(m_Lock);
	return m_Queue.empty();
}

template<class T>
void SafeQueue<T>::Clear()
{
	SingleMutex s(m_Lock);
	//m_Queue.clear();
	while(!m_Queue.empty())
	{
		m_Queue.pop();	
		m_FullSemaphore.Wait();
		m_EmptySemaphore.Post();
	}
}

} //namespace DHT

#endif // !defined(AFX_SAFEQUEUE_H__9EB09358_A23A_41C2_899C_B73E9AC2632C__INCLUDED_)

