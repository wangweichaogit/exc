// SafeSet.h: interface for the SafeSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAFESET_H__0DBFA6E2_28C4_4BB4_BD67_114BC438A8C3__INCLUDED_)
#define AFX_SAFESET_H__0DBFA6E2_28C4_4BB4_BD67_114BC438A8C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>

#include "comLib/comdefine.h" 
#include "comLib/Mutex.h"

namespace DHT
{
template<class T> class SafeSet
{
public:
	typedef typename std::set<T>::iterator iterator;
	typedef typename std::set<T>::size_type size_type;
	SafeSet();
	virtual ~SafeSet();
	bool Find(const T& key);
	bool Insert(const T& key);
	bool Erase(const T& key);
	bool Pop(T& ret_value);

	size_t GetSize();
	bool IsEmpty();
	void Clear();
protected:
	std::set<T> m_Set;
	Mutex m_Lock;

public:
	template<class K, class F>
	bool Search(const K &key, F function)
	{
		SingleMutex s(m_Lock);
		for(iterator it = m_Set.begin(); it != m_Set.end(); it++)
		{
			if(function(*it, key))
			{
				return true;
			}
		}
		return false;
	}

	template<class K, class F>
	bool Search(const K &key, F function, T &value)
	{
		SingleMutex s(m_Lock);
		for(iterator it = m_Set.begin(); it != m_Set.end(); it++)
		{
			if(function(*it, key))
			{
				value = *it;
				return true;
			}
		}
		return false;
	}

	template<class F>
	void DoSomething(F function)
	{
		SingleMutex s(m_Lock);
		for(iterator it = m_Set.begin(); it != m_Set.end(); it++)
		{
			function(*it);
		}
	}

};

template<class T>
SafeSet<T>::SafeSet()
{
}

template<class T>
SafeSet<T>::~SafeSet()
{
}
template<class T>
size_t SafeSet<T>::GetSize()
{
	SingleMutex s(m_Lock);
	return m_Set.size();
}

template<class T>
bool SafeSet<T>::IsEmpty()
{
	SingleMutex s(m_Lock);
	return m_Set.empty();
}

template<class T>
void SafeSet<T>::Clear()
{
	SingleMutex s(m_Lock);
	m_Set.clear();
}

template<class T>
bool SafeSet<T>::Insert(const T &key)
{
	SingleMutex s(m_Lock);
//	std::pair<typename std::set<T>::iterator, bool> p = m_Set.insert(key);
	std::pair<iterator, bool> p = m_Set.insert(key);
	return p.second;
}

template<class T>
bool SafeSet<T>::Erase(const T &key)
{
	SingleMutex s(m_Lock);
//	typename std::set<T>::size_type size = m_Set.erase(key);
	size_type size = m_Set.erase(key);
	return size != 0;
}

template<class T>
bool SafeSet<T>::Find(const T &key)
{
	SingleMutex s(m_Lock);
	iterator it = m_Set.find(key);
	return it != m_Set.end();
}

template<class T>
bool SafeSet<T>::Pop(T& ret_value)
{
	SingleMutex s(m_Lock);
	if(m_Set.empty())
		return false;
	iterator iter = m_Set.begin();
	ret_value = *iter;
	m_Set.erase(iter);
	return true;
}

} //namespace VOIP
#endif // !defined(AFX_SAFESET_H__0DBFA6E2_28C4_4BB4_BD67_114BC438A8C3__INCLUDED_)
