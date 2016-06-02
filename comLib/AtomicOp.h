// AtomicOp.h: interface for the AtomicOp class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_ATOMICOP_H__CCE963DF_A71C_4D4A_A076_574558BD0637__INCLUDED_)
#define AFX_ATOMICOP_H__CCE963DF_A71C_4D4A_A076_574558BD0637__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comLib/Mutex.h"

namespace DHT
{

template <class TYPE>
class AtomicOp
{
public:
	AtomicOp();
	AtomicOp (const TYPE &v);
	virtual ~AtomicOp();

	/// Atomically pre-increment <value_>.
	TYPE operator++ ();
	/// Atomically post-increment <value_>.
	TYPE operator++ (int);
	/// Atomically increment <value_> by rhs.
	TYPE operator+= (const TYPE &rhs);

	/// Atomically pre-decrement <value_>.
	TYPE operator-- ();
	/// Atomically post-decrement <value_>.
	TYPE operator-- (int);
	/// Atomically decrement <value_> by rhs.
	TYPE operator-= (const TYPE &rhs);

	TYPE value () const;

private:
	/// Type of synchronization mechanism.
	mutable Mutex m_lock;
	/// Current object decorated by the atomic op.
	TYPE m_value;
};

template <class TYPE>
AtomicOp<TYPE>::AtomicOp ():m_value(0)
{
}

template <class TYPE>
AtomicOp<TYPE>::~AtomicOp ()
{
}

template <class TYPE>
AtomicOp<TYPE>::AtomicOp (const TYPE &v):m_value(v)
{
}

template <class TYPE> 
TYPE AtomicOp<TYPE>::operator++ ()
{
	SingleMutex  lock(m_lock);
	return ++this->m_value;
}

template <class TYPE> 
TYPE AtomicOp<TYPE>::operator++ (int)
{
	SingleMutex  lock(m_lock);
	return this->m_value++;
}

template <class TYPE> 
TYPE AtomicOp<TYPE>::operator+= (const TYPE &rhs)
{
	SingleMutex  lock(m_lock);
	return this->m_value += rhs;
}

template <class TYPE> 
TYPE AtomicOp<TYPE>::operator-- ()
{
	SingleMutex  lock(m_lock);
	return --this->m_value;
}
template <class TYPE> 
TYPE AtomicOp<TYPE>::operator-- (int)
{
	SingleMutex  lock(m_lock);
	return this->m_value--;
}

template <class TYPE> 
TYPE AtomicOp<TYPE>::operator-= (const TYPE &rhs)
{
	SingleMutex  lock(m_lock);
	return this->m_value -= rhs;
}

template <class TYPE> 
TYPE AtomicOp<TYPE>::value () const
{
	SingleMutex  lock(m_lock);
	return this->m_value;
}

} //namespace IPERFDLL
#endif // !defined(AFX_ATOMICOP_H__CCE963DF_A71C_4D4A_A076_574558BD0637__INCLUDED_)
