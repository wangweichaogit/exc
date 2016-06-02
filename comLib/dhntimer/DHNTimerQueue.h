#ifndef __DHN_TIMER_QUEUE_H__
#define __DHN_TIMER_QUEUE_H__

#include "comLib/Mutex.h"
#include "comLib/dhntimer/DHNTimerDefine.h"
#include "comLib/dhntimer/Time_Value.h"
#include "comLib/dhntimer/Free_List.h"

namespace DHT
{

// class DHNTimerTask;

template <class TYPE>
class DHN_Timer_Node_T
{
public:
	/// Default constructor
	DHN_Timer_Node_T (void){}

	/// Destructor
	~DHN_Timer_Node_T (void){}

	/// Singly linked list
	void set (const TYPE &type,
		const void *a1,
		const void *a2,
		const TimeValue &t,
		const TimeValue &i,
		DHN_Timer_Node_T<TYPE> *n,
		long timer_id);

	/// Doubly linked list version
	void set (const TYPE &type,
		const void *a1,
		const void *a2,
		const TimeValue &t,
		const TimeValue &i,
		DHN_Timer_Node_T<TYPE> *p,
		DHN_Timer_Node_T<TYPE> *n,
		long timer_id);

	// = Accessors

	/// Get the type.
	TYPE &get_type (void);

	/// Set the type.
	void set_type (TYPE &type);

	/// Get param1.
	const void *get_act1 (void);

	/// Get param2.
	const void *get_act2 (void);

	/// Set param1.
	void set_act1 (void *act);

	/// Set param2.
	void set_act2 (void *act);

	/// Get the timer value.
	const TimeValue &get_timer_value (void) const;

	/// Set the timer value.
	void set_timer_value (const TimeValue &timer_value);

	/// Get the timer interval.
	const TimeValue &get_interval (void) const;

	/// Set the timer interval.
	void set_interval (const TimeValue &interval);

	/// Get the previous pointer.
	DHN_Timer_Node_T<TYPE> *get_prev (void);

	/// Set the previous pointer.
	void set_prev (DHN_Timer_Node_T<TYPE> *prev);

	/// Get the next pointer.
	DHN_Timer_Node_T<TYPE> *get_next (void);

	/// Set the next pointer.
	void set_next (DHN_Timer_Node_T<TYPE> *next);

	/// Get the timer_id.
	long get_timer_id (void) const;

	/// Set the timer_id.
	void set_timer_id (long timer_id);

private:
	/// Type of object stored in the Queue
	TYPE type_;

	/// Param1.
	const void *act1_;

	/// Param2.
	const void *act2_;

	/// Time until the timer expires.
	TimeValue timer_value_;

	/// If this is a periodic timer this holds the time until the next
	/// timeout.
	TimeValue interval_;

	/// Pointer to previous timer.
	DHN_Timer_Node_T<TYPE> *prev_;

	/// Pointer to next timer.
	DHN_Timer_Node_T<TYPE> *next_;

	/// Id of this timer (used to cancel timers before they expire).
	long timer_id_;
};

template <class TYPE>
class TimerQueue
{
public:
	TimerQueue(DHN_Free_List<DHN_Timer_Node_T <TYPE> > *freelist = 0);
	virtual ~TimerQueue();

public:
	virtual long schedule(
		TYPE type,
		const void *act1,
		const void *act2,
		const TimeValue &tAfter,
		const TimeValue &tInterval = TimeValue::zero);

	virtual long schedule_i (const TYPE &type,
		const void *act1,
		const void *act2,
		const TimeValue &future_time,
		const TimeValue &interval) = 0;

	virtual int cancel_timer(long timer_id,	const void** act1 = 0,const void** act2 = 0,int dont_call_handle_close = 1) = 0;
	virtual DHN_Timer_Node_T<TYPE> *alloc_node (void);
	TimeValue gettimeofday (void);
	void gettimeofday (TimeValue (*gettimeofday)(void));
	virtual TimeValue *calculate_timeout (TimeValue *max,TimeValue *the_timeout);
	virtual bool is_empty (void) const = 0;
	int expire (void);
	int expire (const TimeValue& cur_time);
	virtual int dispatch_info_i (const TimeValue &current_time);
	virtual DHN_Timer_Node_T<TYPE> *remove_first (void) = 0;
	virtual void reschedule (DHN_Timer_Node_T<TYPE> *) = 0;
	virtual const TimeValue &earliest_time (void) const = 0;
	virtual void free_node (DHN_Timer_Node_T<TYPE> *node) = 0;
	DHN_Free_List<DHN_Timer_Node_T<TYPE> > *free_list_;
	TimeValue (*gettimeofday_)(void);

private:
	bool const delete_free_list_;

protected:
	Mutex m_mutex;
};

//////////////////////////////////////////////////////////////////////////
template <class TYPE> 
inline void DHN_Timer_Node_T<TYPE>::set (
	const TYPE &type,
	const void *a1,
	const void *a2,
	const TimeValue &t,
	const TimeValue &i,
	DHN_Timer_Node_T<TYPE> *n,
	long timer_id)
{
	this->type_ = type;
	this->act1_ = a1;
	this->act2_ = a2;
	this->timer_value_ = t;
	this->interval_ = i;
	this->next_ = n;
	this->timer_id_ = timer_id;
}

template <class TYPE> 
inline void DHN_Timer_Node_T<TYPE>::set (const TYPE &type,
	const void *a1,
	const void *a2,
	const TimeValue &t,
	const TimeValue &i,
	DHN_Timer_Node_T<TYPE> *p,
	DHN_Timer_Node_T<TYPE> *n,
	long timer_id)
{
	this->type_ = type;
	this->act1_ = a1;
	this->act2_ = a2;
	this->timer_value_ = t;
	this->interval_ = i;
	this->prev_ = p;
	this->next_ = n;
	this->timer_id_ = timer_id;
}

template <class TYPE> 
inline TYPE &DHN_Timer_Node_T<TYPE>::get_type (void)
{
	return this->type_;
}

template <class TYPE> 
inline void DHN_Timer_Node_T<TYPE>::set_type (TYPE &type)
{
	this->type_ = type;
}

template <class TYPE> 
inline const void * DHN_Timer_Node_T<TYPE>::get_act1 (void)
{
	return this->act1_;
}

template <class TYPE> 
inline const void * DHN_Timer_Node_T<TYPE>::get_act2 (void)
{
	return this->act2_;
}

template <class TYPE> 
inline void DHN_Timer_Node_T<TYPE>::set_act1 (void *act)
{
	this->act1_ = act;
}

template <class TYPE> 
inline void DHN_Timer_Node_T<TYPE>::set_act2 (void *act)
{
	this->act2_ = act;
}

template <class TYPE> 
inline const TimeValue &DHN_Timer_Node_T<TYPE>::get_timer_value (void) const
{
	return this->timer_value_;
}

template <class TYPE> 
inline void DHN_Timer_Node_T<TYPE>::set_timer_value (const TimeValue &timer_value)
{
	this->timer_value_ = timer_value;
}

template <class TYPE> 
inline const TimeValue &DHN_Timer_Node_T<TYPE>::get_interval (void) const
{
	return this->interval_;
}

template <class TYPE> 
inline void DHN_Timer_Node_T<TYPE>::set_interval (const TimeValue &interval)
{
	this->interval_ = interval;
}

template <class TYPE> 
inline DHN_Timer_Node_T<TYPE> *DHN_Timer_Node_T<TYPE>::get_prev (void)
{
	return this->prev_;
}

template <class TYPE> 
inline void DHN_Timer_Node_T<TYPE>::set_prev (DHN_Timer_Node_T<TYPE> *prev)
{
	this->prev_ = prev;
}

template <class TYPE> 
inline DHN_Timer_Node_T<TYPE> *DHN_Timer_Node_T<TYPE>::get_next (void)
{
	return this->next_;
}

template <class TYPE> 
inline void DHN_Timer_Node_T<TYPE>::set_next (DHN_Timer_Node_T<TYPE> *next)
{
	this->next_ = next;
}

template <class TYPE> 
inline long DHN_Timer_Node_T<TYPE>::get_timer_id (void) const
{
	return this->timer_id_;
}

template <class TYPE> 
inline void DHN_Timer_Node_T<TYPE>::set_timer_id (long timer_id)
{
	this->timer_id_ = timer_id;
}

///////////////////////////////////schedule_timer///////////////////////////////////////
template <class TYPE>
TimerQueue<TYPE>::TimerQueue(DHN_Free_List<DHN_Timer_Node_T <TYPE> > *freelist): 
gettimeofday_ (TimeValue::gettimeofday),
	delete_free_list_ (freelist == 0)
{
	if (!freelist)
		free_list_ = new DHN_Locked_Free_List<DHN_Timer_Node_T<TYPE> >;
	else
		free_list_ = freelist;
}

template <class TYPE>
TimerQueue<TYPE>::~TimerQueue()
{
	if (free_list_ != NULL)
	{
		delete free_list_;
	}
}

template <class TYPE> 
inline TimeValue TimerQueue<TYPE>::gettimeofday (void)
{
	// Invoke gettimeofday via pointer to function.
	return this->gettimeofday_ ();
}

template <class TYPE> 
inline TimeValue* TimerQueue<TYPE>::calculate_timeout (TimeValue *max_wait_time, TimeValue *the_timeout)
{
	SingleMutex s(m_mutex);
	if (the_timeout == 0)
		return max_wait_time;

	if (this->is_empty ())
	{
		// Nothing on the Timer_Queue, so use whatever the caller gave us.
		if (max_wait_time)
		{
			*the_timeout = *max_wait_time;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		TimeValue cur_time = this->gettimeofday ();
		TimeValue t_earliest_time = this->earliest_time();

		if (t_earliest_time > cur_time)
		{
			// The earliest item on the Timer_Queue is still in the
			// future.  Therefore, use the smaller of (1) caller's wait
			// time or (2) the delta time between now and the earliest
			// time on the Timer_Queue.
			// 
			// 			g_logger.Debug("t_earliest_time:%I64ds %ldus,cur_time:%I64ds %ldus",
			// 				t_earliest_time.sec(),
			// 				t_earliest_time.usec(),
			// 				cur_time.sec(),
			// 				cur_time.usec());

			*the_timeout = t_earliest_time - cur_time;
			if (!(max_wait_time == 0 || *max_wait_time > *the_timeout))
			{
				*the_timeout = *max_wait_time;
			}
		}
		else
		{
			// The earliest item on the Timer_Queue is now in the past.
			// Therefore, we've got to "poll" the Reactor, i.e., it must
			// just check the descriptors and then dispatch timers, etc.
			*the_timeout = TimeValue::zero;
		}
	}

	return the_timeout;
}

template <class TYPE> 
inline void TimerQueue<TYPE>::gettimeofday (TimeValue (*gettimeofday)(void))
{
	this->gettimeofday_ = gettimeofday;
}

template <class TYPE> 
int TimerQueue<TYPE>::expire (void)
{
	SingleMutex s(m_mutex);
	if (!this->is_empty ())
		return this->expire (this->gettimeofday () /*+ timer_skew_*/);
	else
		return 0;
}

template <class TYPE> 
int TimerQueue<TYPE>::expire (const TimeValue& cur_time)
{
	if (this->is_empty ())
	{
		return 0;
	}

	int number_of_timers_expired = 0;
	int result = 0;

	while ((result = this->dispatch_info_i (cur_time)) != 0)
	{
		++number_of_timers_expired;
	}

	return number_of_timers_expired;
}

template <class TYPE> 
int TimerQueue<TYPE>::dispatch_info_i (const TimeValue &cur_time)
{
	if (this->is_empty ())
	{
		return 0;
	}

	DHN_Timer_Node_T<TYPE> *expired = 0;

	TimeValue earliest = this->earliest_time ();

	if (earliest <= cur_time)
	{
		expired = this->remove_first ();
		if (expired == NULL)
		{
			return 0;
		}

		TYPE ev = expired->get_type();
		if (ev == NULL)
		{
			return 0;
		}

		TimeValue tv = this->gettimeofday();
		ev->handle_timeout(expired->get_timer_id(),tv,expired->get_act1(),expired->get_act2());

		// Check if this is an interval timer.
		TimeValue tExpires = expired->get_interval();
		if (tExpires > TimeValue::zero)
		{
			// Make sure that we skip past values that have already
			// "expired".
			do
			{
				TimeValue to = expired->get_timer_value () + expired->get_interval ();
				expired->set_timer_value (to);
			}
			while (expired->get_timer_value () <= cur_time);

			// Since this is an interval timer, we need to reschedule
			// it.
			if (ev->is_stop_repeated())
			{
				this->free_node (expired);
				ev->handle_repeated_timer_stoped();
			}
			else
			{
				this->reschedule (expired);
			}
		}
		else
		{
			// Call the factory method to free up the node.
			this->free_node (expired);
		}

		return 1;
	}

	return 0;
}

template <class TYPE> 
DHN_Timer_Node_T<TYPE> *TimerQueue<TYPE>::alloc_node (void)
{
	return this->free_list_->remove ();
}

template <class TYPE> 
long TimerQueue<TYPE>::schedule(
	TYPE type,
	const void *act1,
	const void *act2,
	const TimeValue &tAfter,
	const TimeValue &tInterval)
{
	SingleMutex s(m_mutex);
	return this->schedule_i(type,act1,act2,tAfter,tInterval);
}
};

// #include "DHNTimerQueue.cpp"

#endif
