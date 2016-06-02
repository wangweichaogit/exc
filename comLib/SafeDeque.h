// SafeDeque.h
//
//////////////////////////////////////////////////////////////////////

#ifndef __SAFE_DEQUE_H_
#define __SAFE_DEQUE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <deque>

#include "comLib/comdefine.h" 
#include "comLib/Mutex.h"

namespace DHT
{

template<class T> class SafeDeque 
{
	enum{MAX_DEFAULT_SIZE=16*1024, MAX_QUEUE_SIZE=4000*1024};
public:
	SafeDeque(size_t queue_size = MAX_DEFAULT_SIZE);
	virtual ~SafeDeque();
	bool Pop_Front(T& ret_value);
	bool Pop_Back(T& ret_value);
	void Push_Front(const T& value);
	void Push_Back(const T& value);
	size_t GetSize() const;
	bool IsEmpty() const;
	void Clear();
	const T& Front() const;
	const T& Back() const;
protected:
	std::deque<T> m_Queue;
	mutable Mutex m_Lock;
	size_t m_queue_size;
};


//////////////////////////////////////////////////
//------------Block SafeDeque----------------
//////////////////////////////////////////////////////////////////////
template<class T>
SafeDeque<T>::SafeDeque(size_t queue_size) :
m_queue_size(queue_size > MAX_QUEUE_SIZE ? MAX_QUEUE_SIZE : queue_size)
{
}

template<class T>
SafeDeque<T>::~SafeDeque() 
{
}

template<class T>
bool SafeDeque<T>::Pop_Front(T& ret_value)
{
	SingleMutex s(m_Lock);
	if(m_Queue.size() > 0)
	{
		ret_value = m_Queue.front();
		m_Queue.pop_front();
		return true;
	}
	return false;
}

template<class T>
bool SafeDeque<T>::Pop_Back(T& ret_value)
{
	SingleMutex s(m_Lock);
	if(m_Queue.size() > 0)
	{
		ret_value = m_Queue.back();
		m_Queue.pop_back();
		return true;
	}
	return false;
}

template<class T>
void SafeDeque<T>::Push_Front(const T& value)
{
	SingleMutex s(m_Lock);
	if(m_Queue.size() >= m_queue_size)//maybe overflow, need log out
	{
		m_Queue.pop_back();
	}
	m_Queue.push_front(value);
}

template<class T>
void SafeDeque<T>::Push_Back(const T& value)
{
	SingleMutex s(m_Lock);
	if(m_Queue.size() >= m_queue_size)//maybe overflow, need log out
	{
		m_Queue.pop_front();
	}
	m_Queue.push_back(value);
}

template<class T>
size_t SafeDeque<T>::GetSize() const
{
	SingleMutex s(m_Lock);
	return m_Queue.size();
}

template<class T>
bool SafeDeque<T>::IsEmpty() const
{
	SingleMutex s(m_Lock);
	return m_Queue.empty();
}

template<class T>
void SafeDeque<T>::Clear()
{
	SingleMutex s(m_Lock);
	m_Queue.clear();
}

template<class T>
const T& SafeDeque<T>::Front() const
{
	SingleMutex s(m_Lock);
	return m_Queue.front();
}

template<class T>
const T& SafeDeque<T>::Back() const
{
	SingleMutex s(m_Lock);
	return m_Queue.back();
}

} //namespace VOIP

#endif

