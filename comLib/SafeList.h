// SafeList.h: interface for the SafeList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAFELIST_H__F5055174_9D36_4013_931B_9186A580E76A__INCLUDED_)
#define AFX_SAFELIST_H__F5055174_9D36_4013_931B_9186A580E76A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <list>

#include "comLib/comdefine.h" 
#include "comLib/Mutex.h"
#include "comLib/Semaphore.h"

namespace DHT
{
template<class T>
class SafeList  
{
	enum{MAX_DEFAULT_SIZE=16*1024, MAX_LIST_COUNT=4000*1024};
public:
	typedef typename std::list<T>::iterator iterator;
	SafeList(size_t max_size = MAX_DEFAULT_SIZE);
	virtual ~SafeList();
	bool Back(T &value);
	bool Front(T &value);
	bool Pop_back();
	bool Pop_front();
	bool Push_back(const T &value);
	bool Push_front(const T &value);
	bool Pop(T& ret_value, int timeout);
	void Push(const T& value);

	size_t GetSize();
	bool IsEmpty();
	void Clear();
protected:
	std::list<T> m_List;
	Mutex m_Lock;
	size_t m_max_count;
	Semaphore m_EmptySemaphore;
public:
	template<class F>
	void DoSomething(F function) // F is callback function, loopmap function(it->first, it->second, value);
	{
		SingleMutex s(m_Lock);
		for(iterator it = m_List.begin(); it != m_List.end(); it++)
		{
			function(*it);
		}
	}

	template<class F, class V>
	void DoSomething(F function, const V &value) // F is callback function, loopmap function(it->first, it->second, value);
	{
		SingleMutex s(m_Lock);
		for(iterator it = m_List.begin(); it != m_List.end(); it++)
		{
			function(*it,value);
		}
	}
	template<class F, class K>  T Find(F function, K& match)
	{
		SingleMutex s(m_Lock);
		for(iterator it = m_List.begin(); it != m_List.end(); it++)
		{
			if(function(*it, match))
			{
				return *it;
			}
		}
		return NULL;
	}

	template<class F>  void Enum(F function)
	{
		SingleMutex s(m_Lock);
		int size = GetSize();
		for(int i = 0; i < size; i++)
		{
			T value;
			if(Front(value))
			{
				Pop_front();
				function(value);
			}
		}
	}

	void Erase(T& key)
	{
		SingleMutex s(m_Lock);
		m_List.remove(key);
	}

	bool Exist(T& key)
	{
		SingleMutex s(m_Lock);
		iterator itr;
		itr = std::find(m_List.begin(), m_List.end(), key);
		return (itr != m_List.end());
	}
};
//////////////////////////////////////////////////////////////////////
template<class T>
SafeList<T>::SafeList(size_t max_size) :
#ifdef WIN32
m_max_count(max_size > MAX_LIST_COUNT ? MAX_LIST_COUNT : max_size), 
#else
m_max_count(max_size > SEM_VALUE_MAX ? SEM_VALUE_MAX : max_size), 
#endif
m_EmptySemaphore(0,m_max_count)
{
}
template<class T>
SafeList<T>::~SafeList() 
{
}
template<class T>
size_t SafeList<T>::GetSize()
{
	SingleMutex s(m_Lock);
	return m_List.size();
}

template<class T>
bool SafeList<T>::IsEmpty()
{
	SingleMutex s(m_Lock);
	return m_List.empty();
}

template<class T>
void SafeList<T>::Clear()
{
	SingleMutex s(m_Lock);
	while(!m_List.empty())
	{
		m_List.pop_front();	
		m_EmptySemaphore.Wait();
	}
}
template<class T>
bool SafeList<T>::Back(T &value)
{
	SingleMutex s(m_Lock);
	if(m_List.empty())
		return false;
	value = m_List.back();
	return true;
}

template<class T>
bool SafeList<T>::Front(T &value)
{
	SingleMutex s(m_Lock);
	if(m_List.empty())
		return false;
	value = m_List.front();
	return true;
}

template<class T>
bool SafeList<T>::Pop_back()
{
	SingleMutex s(m_Lock);
	if(m_List.empty())
		return false;
	m_EmptySemaphore.Wait();//等待空位置
	m_List.pop_back();
	return true;
}

template<class T>
bool SafeList<T>::Pop_front()
{
	SingleMutex s(m_Lock);
	if(m_List.empty())
		return false;
	m_EmptySemaphore.Wait();//等待空位置
	m_List.pop_front();
	return true;
}

template<class T>
bool SafeList<T>::Push_back(const T &value)
{
	SingleMutex s(m_Lock);
	if(m_List.size() >= m_max_count)//maybe overflow, need log out
	{
		return false;
	}
	else
	{
		m_List.push_back(value);
		m_EmptySemaphore.Post();
		return true;
	}
}

template<class T>
bool SafeList<T>::Push_front(const T &value)
{
	SingleMutex s(m_Lock);
	if(m_List.size() >= m_max_count)//maybe overflow, need log out
	{
		return false;
	}
	else
	{
		m_List.push_front(value);
		m_EmptySemaphore.Post();
		return true;
	}
}
template<class T>
bool SafeList<T>::Pop(T& ret_value, int timeout)
{
	if (true == m_EmptySemaphore.Wait(timeout))// have  -1
	{
		SingleMutex s(m_Lock);
		ret_value = m_List.front();
		m_List.pop_front();
		return true;
	}
	return false;
}
template<class T>
void SafeList<T>::Push(const T& value)
{
	SingleMutex s(m_Lock);
	if(m_List.size() >= m_max_count)//maybe overflow, need log out
	{
		m_List.pop_front();	
		m_EmptySemaphore.Wait();
	}

	m_List.push_back(value);
	m_EmptySemaphore.Post();	//+1
}

} //namespace VOIP
#endif // !defined(AFX_SAFELIST_H__F5055174_9D36_4013_931B_9186A580E76A__INCLUDED_)
