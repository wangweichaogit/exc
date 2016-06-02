// SafeMap.h: interface for the SafeMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAFEMAP_H__90447658_EB92_49E5_B8E0_99C38E21EEF6__INCLUDED_)
#define AFX_SAFEMAP_H__90447658_EB92_49E5_B8E0_99C38E21EEF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>

#include "comLib/comdefine.h" 
#include "comLib/Mutex.h"

namespace DHT
{
template <class KEY, class T> class SafeMap 
{
public:
	typedef typename std::map<KEY, T>::iterator iterator;
	SafeMap();
	virtual ~SafeMap();
	bool Find(const KEY& key);
	bool Find(const KEY& key, T & ret_t);
	bool FindAndErase(const KEY& key, T & ret_t);
	bool Insert(const KEY& key, const T& value);
	bool Erase(const KEY& key);
	T &operator[](const KEY &key);

	size_t GetSize();
	bool IsEmpty();
	void Clear();

protected:
	std::map<KEY, T> m_Map;
	Mutex m_Lock;

public:
	template<class OUTK, class F>
	bool Search(const OUTK &key, F function) // F is callback function, loopmap if (function(it->second, OUTK)) return true
	{
		SingleMutex s(m_Lock);
		for(iterator it = m_Map.begin(); it != m_Map.end(); it++)
		{
			if(function(it->second, key))
			{
				return true;
			}
		}
		return false;
	}
	template<class OUTK, class F>
	bool Search(const OUTK &key, F function, T &value) // F is callback function, loopmap if (function(it->second, OUTK)) return true
	{
		SingleMutex s(m_Lock);
		for(iterator it = m_Map.begin(); it != m_Map.end(); it++)
		{
			if(function(it->second, key))
			{
				value = it->second;
				return true;
			}
		}
		return false;
	}

    //根据第二项来查找第一项
    template<class OUTK, class F>
    bool Search2(const OUTK &key, F function, KEY &value) // F is callback function, loopmap if (function(it->second, OUTK)) return true
    {
        SingleMutex s(m_Lock);
        for(iterator it = m_Map.begin(); it != m_Map.end(); it++)
        {
            if(function(it->second, key))
            {
                value = it->first;
                return true;
            }
        }
        return false;
    }
    //
	template<class F>
	void Remove(F function) // F is callback function, loopmap if(function(it->second)) erase(it);
	{
		SingleMutex s(m_Lock);
		for(iterator it = m_Map.begin(); it != m_Map.end(); )
		{
			if(function(it->second))
				m_Map.erase(it++);
			else
				it++;  //不能放到for(;;it++)中
		}
	}
	template<class F, class V>
	void DoSomething(F function, const V &value) // F is callback function, loopmap function(it->first, it->second, value);
	{
		SingleMutex s(m_Lock);
		for(iterator it = m_Map.begin(); it != m_Map.end(); it++)
		{
			function(it->first, it->second, value);
		}
	}
};

//////////////////////////////////////////////////////////////////////
template<class KEY, class T>
SafeMap<KEY, T>::SafeMap()
{
}

template<class KEY, class T>
SafeMap<KEY, T>::~SafeMap()
{
}

template<class KEY, class T>
size_t SafeMap<KEY, T>::GetSize()
{
	SingleMutex s(m_Lock);
	return m_Map.size();
}

template<class KEY, class T>
bool SafeMap<KEY, T>::IsEmpty()
{
	SingleMutex s(m_Lock);
	return m_Map.empty();
}

template<class KEY, class T>
void SafeMap<KEY, T>::Clear()
{
	SingleMutex s(m_Lock);
	m_Map.clear();
}


template<class KEY, class T>
bool SafeMap<KEY, T>::Find(const KEY &key)
{
	SingleMutex s(m_Lock);
	iterator iter = m_Map.find(key);
	return (iter != m_Map.end());
}

template<class KEY, class T>
bool SafeMap<KEY, T>::Find(const KEY& key, T & ret_t)
{
	SingleMutex s(m_Lock);
	iterator iter = m_Map.find(key);
	if(m_Map.end() != iter)
	{
		ret_t = iter->second;
		return true;
	}
	return false;
}

template<class KEY, class T>
bool SafeMap<KEY, T>::FindAndErase(const KEY& key, T & ret_t)
{
	SingleMutex s(m_Lock);
	iterator iter = m_Map.find(key);
	if(m_Map.end() != iter)
	{
		ret_t = iter->second;
		m_Map.erase(key);
		return true;
	}
	return false;
}

template<class KEY, class T>
bool SafeMap<KEY, T>::Insert(const KEY &key, const T &value)
{
	SingleMutex s(m_Lock);
#if defined(_MSC_VER) && (_MSC_VER<1300)	//VC6
	std::pair<std::map<KEY, T>::iterator, bool> pair = m_Map.insert(std::map<KEY, T>::value_type(key, value));
#else
	std::pair<typename std::map<KEY, T>::iterator, bool> pair = m_Map.insert(typename std::map<KEY, T>::value_type(key, value));
#endif
	return pair.second;
}

template<class KEY, class T>
bool SafeMap<KEY, T>::Erase(const KEY &key)
{
	SingleMutex s(m_Lock);
	typename std::map<KEY, T>::size_type size = m_Map.erase(key);
	return size != 0;
}

template<class KEY, class T>
T &SafeMap<KEY, T>::operator[](const KEY &key)
{
	SingleMutex s(m_Lock);
	return m_Map[key];
}


} //namespace VOIP
#endif // !defined(AFX_SAFEMAP_H__90447658_EB92_49E5_B8E0_99C38E21EEF6__INCLUDED_)
