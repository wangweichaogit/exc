#ifndef __DHN_FREE_LIST_H__
#define __DHN_FREE_LIST_H__

#include "comLib/Mutex.h"
#include "comLib/dhntimer/DHNTimerDefine.h"

namespace DHT
{
template <class T>
class DHN_Free_List
{
public:
  /// Destructor - removes all the elements from the free_list.
  virtual ~DHN_Free_List (void);

  /// Inserts an element onto the free list (if it isn't past the high
  /// water mark).
  virtual void add (T *element) = 0;

  /// Takes a element off the freelist and returns it.  It creates
  /// <inc> new elements if the size is at or below the low water mark.
  virtual T *remove (void) = 0;

  /// Returns the current size of the free list.
  virtual size_t size (void) = 0;

  /// Resizes the free list to @a newsize.
  virtual void resize (size_t newsize) = 0;
};

template <class T>
class DHN_Locked_Free_List : public DHN_Free_List<T>
{
public:
  DHN_Locked_Free_List (int mode = DHN_FREE_LIST_WITH_POOL,
                        size_t prealloc = DHN_DEFAULT_FREE_LIST_PREALLOC,
                        size_t lwm = DHN_DEFAULT_FREE_LIST_LWM,
                        size_t hwm = DHN_DEFAULT_FREE_LIST_HWM,
                        size_t inc = DHN_DEFAULT_FREE_LIST_INC);

  /// Destructor - removes all the elements from the free_list.
  virtual ~DHN_Locked_Free_List (void);

  /// Inserts an element onto the free list (if it isn't past the high
  /// water mark).
  virtual void add (T *element);

  /// Takes a element off the freelist and returns it.  It creates
  /// <inc> new elements if the size is at or below the low water mark.
  virtual T *remove (void);

  /// Returns the current size of the free list.
  virtual size_t size (void);

  /// Resizes the free list to @a newsize.
  virtual void resize (size_t newsize);

protected:
  /// Allocates @a n extra nodes for the freelist.
  virtual void alloc (size_t n);

  /// Removes and frees @a n nodes from the freelist.
  virtual void dealloc (size_t n);

  /// Free list operation mode, either ACE_FREE_LIST_WITH_POOL or
  /// ACE_PURE_FREE_LIST.
  int mode_;

  /// Pointer to the first node in the freelist.
  T *free_list_;

  /// Low water mark.
  size_t lwm_;

  /// High water mark.
  size_t hwm_;

  /// Increment value.
  size_t inc_;

  /// Keeps track of the size of the list.
  size_t size_;

  /// Synchronization variable for ACE_Timer_Queue.
  DHT::Mutex mutex_;
};

template <class T>
DHN_Free_List<T>::~DHN_Free_List (void)
{
}

template <class T>
DHN_Locked_Free_List<T>::DHN_Locked_Free_List (
	int mode,
	size_t prealloc,
	size_t lwm,
	size_t hwm,
	size_t inc)
	: mode_ (mode),
	free_list_ (0),
	lwm_ (lwm),
	hwm_ (hwm),
	inc_ (inc),
	size_ (0)
{
	this->alloc (prealloc);
}

template <class T>
DHN_Locked_Free_List<T>::~DHN_Locked_Free_List (void)
{
	if (this->mode_ != DHN_PURE_FREE_LIST)
		while (this->free_list_ != 0)
		{
			T *temp = this->free_list_;
			this->free_list_ = this->free_list_->get_next ();
			delete temp;
		}
}

template <class T>
void DHN_Locked_Free_List<T>::add (T *element)
{
	if (this->mode_ == DHN_PURE_FREE_LIST || this->size_ < this->hwm_)
	{
		element->set_next (this->free_list_);
		this->free_list_ = element;
		this->size_++;
	}
	else
	{
		delete element;
	}
}

template <class T> 
T *DHN_Locked_Free_List<T>::remove (void)
{
	if (this->mode_ != DHN_PURE_FREE_LIST && this->size_ <= this->lwm_)
		this->alloc (this->inc_);

	// Remove a node
	T *temp = this->free_list_;

	if (temp != 0)
	{
		this->free_list_ = this->free_list_->get_next ();
		this->size_--;
	}

	return temp;
}

template <class T> 
size_t DHN_Locked_Free_List<T>::size (void)
{
	return this->size_;
}

template <class T> 
void DHN_Locked_Free_List<T>::resize (size_t newsize)
{
	// Check if we are allowed to resize
	if (this->mode_ != DHN_PURE_FREE_LIST)
	{
		// Check to see if we grow or shrink
		if (newsize < this->size_)
		{
			this->dealloc (this->size_ - newsize);
		}
		else
		{
			this->alloc (newsize - this->size_);
		}
	}
}

template <class T> 
void DHN_Locked_Free_List<T>::alloc (size_t n)
{
	for (; n > 0; n--)
	{
		T *temp = 0;
		temp = new T;
		temp->set_next (this->free_list_);
		this->free_list_ = temp;
		this->size_++;
	}
}

template <class T> 
void DHN_Locked_Free_List<T>::dealloc (size_t n)
{
	for (; this->free_list_ != 0 && n > 0;
		n--)
	{
		T *temp = this->free_list_;
		this->free_list_ = this->free_list_->get_next ();
		delete temp;
		this->size_--;
	}
}

};

// #include "Free_List.cpp"

#endif //__DHN_FREE_LIST_H__
