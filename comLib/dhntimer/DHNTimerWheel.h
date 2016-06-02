#ifndef __TIMER_WHEEL_H__
#define __TIMER_WHEEL_H__

#include "comLib/dhntimer/DHNTimerQueue.h"
#include "comLib/dhntimer/Time_Value.h"

namespace DHT
{

template <class TYPE>
class TimerWheel : public TimerQueue<TYPE>
{
public:
	typedef TimerQueue<TYPE> Base;
	typedef DHN_Timer_Node_T<TYPE> Node;
	typedef DHN_Free_List<Node> FreeList;

public:
	TimerWheel(unsigned int spoke_count,
		unsigned int resolution,
		int prealloc = 0,
		FreeList* freelist = 0);
	virtual ~TimerWheel();

public:
	virtual int cancel_timer(
		long timer_id,
		const void** act1 = 0,
		const void** act2 = 0,
		int dont_call_handle_close = 1);
	unsigned int calculate_spoke(const TimeValue& expire) const;

	virtual long schedule_i (const TYPE &type,
		const void *act1,
		const void *act2,
		const TimeValue &future_time,
		const TimeValue &interval);

	void schedule_i(DHN_Timer_Node_T<TYPE>* n, u_int spoke,const TimeValue& expire);

private:
	long generate_timer_id (unsigned int spoke);
	
	DHN_Timer_Node_T<TYPE>* find_spoke_node(unsigned int spoke, long timer_id) const;
	DHN_Timer_Node_T<TYPE>* find_node (long timer_id) const;
	bool is_empty (void) const;
	const TimeValue &earliest_time (void) const;
	DHN_Timer_Node_T<TYPE>* get_first_i (void) const;
	void free_node (DHN_Timer_Node_T<TYPE> *node);
	void cancel_i (DHN_Timer_Node_T<TYPE>* n);
	void unlink (DHN_Timer_Node_T<TYPE>* n);
	void recalc_earliest(const TimeValue& last);
	void open_i(int prealloc, unsigned int spokes, unsigned int res);
	int power2bits (int n, int min_bits, int max_bits);
	virtual DHN_Timer_Node_T<TYPE> *remove_first (void);
	DHN_Timer_Node_T<TYPE>* remove_first_expired (const TimeValue& now);
	virtual DHN_Timer_Node_T<TYPE>* get_first (void);
	virtual void reschedule (DHN_Timer_Node_T<TYPE> *);

private:
	/// Timing Wheel.
	DHN_Timer_Node_T<TYPE>** spokes_;
	/// Size of the timing wheel.
	u_int spoke_count_;
	/// Number of timer_id bits used for the spoke
	int spoke_bits_;
	/// Maximum number of timers per spoke
	u_int max_per_spoke_;
	/// Resolution (in microseconds) of the timing wheel.
	int res_bits_;
	/// Index of the list with the earliest time
	u_int earliest_spoke_;
	/// The total amount of time in one iteration of the wheel. (resolution * spoke_count)
	TimeValue wheel_time_;
	/// The total number of timers currently scheduled.
	u_int timer_count_;

};

template <class TYPE>
TimerWheel<TYPE>::TimerWheel(unsigned int spoke_count,
	unsigned int resolution,
	int prealloc,
	FreeList* freelist)
	:Base (freelist)
	, spokes_ (0)
	, spoke_count_ (0) // calculated in open_i
	, spoke_bits_ (0)
	, res_bits_ (0)
	, earliest_spoke_ (0)
	, timer_count_ (0)

{
	this->open_i (prealloc, spoke_count, resolution);
}

template <class TYPE>
TimerWheel<TYPE>::~TimerWheel()
{
	for (u_int i = 0; i < this->spoke_count_; ++i)
	{
		// Free all the nodes starting at the root
		DHN_Timer_Node_T<TYPE>* root = this->spokes_[i];
		for (DHN_Timer_Node_T<TYPE>* n = root->get_next (); n != root;)
		{
			DHN_Timer_Node_T<TYPE>* next = n->get_next ();
			this->free_node (n);
			n = next;
		}
		delete root;
	}
	delete[] this->spokes_;
}

template <class TYPE> 
int TimerWheel<TYPE>::power2bits (int n,
	int min_bits,
	int max_bits)
{
	int max = (1 << max_bits) - 1;
	if (n > max)
		return max_bits;

	// count the bits in n.
	int i = 0;
	int tmp = n;
	do
	{
		tmp >>= 1;
		++i;
	}
	while (tmp != 0);

	if (i <= min_bits)
		return min_bits;

	// Which is nearest?
	int a = (1 << i) - n;
	int b = (1 << (i - 1)) - n;
	if (b < 0)
		b = -b;
	if (b < a)
		return i - 1;
	return i;
}

template <class TYPE>
void TimerWheel<TYPE>::open_i(int prealloc, unsigned int spokes, unsigned int res)
{
	this->gettimeofday (TimeValue::gettimeofday);

	// Rather than waste bits in our timer id, we might as well round up
	// the spoke count to the next power of two - 1 . (i.e 1,3,7,15,...127,etc.)
	const int MIN_SPOKE_BITS = 3;  // Allow between 8 and 4096 spokes
	const int MAX_SPOKE_BITS = 12;
	const int MAX_RES_BITS = 20;   // 20 is plenty, even on 64 bit platforms.

	this->spoke_bits_ = power2bits (spokes, MIN_SPOKE_BITS, MAX_SPOKE_BITS);
	this->res_bits_ = power2bits (res, 1, MAX_RES_BITS);

	this->spoke_count_ = 1 << this->spoke_bits_;

	this->free_list_->resize (prealloc + this->spoke_count_);

	this->wheel_time_.msec (1 << (this->res_bits_));

	this->spokes_ = new DHN_Timer_Node_T<TYPE>* [this->spoke_count_];

	// Create the root nodes. These will be treated specially
	for (u_int i = 0; i < this->spoke_count_; ++i)
	{
		DHN_Timer_Node_T<TYPE>* root = this->alloc_node ();
		root->set (0, 0, 0, TimeValue::zero, TimeValue::zero, root, root, 0);
		this->spokes_[i] = root;
	}

}

template <class TYPE> 
void TimerWheel<TYPE>::free_node (DHN_Timer_Node_T<TYPE> *node)
{
	this->free_list_->add (node);
}

template <class TYPE> 
void TimerWheel<TYPE>::unlink (DHN_Timer_Node_T<TYPE>* n)
{
	--timer_count_;
	n->get_prev ()->set_next (n->get_next ());
	n->get_next ()->set_prev (n->get_prev ());
	n->set_prev (0);
	n->set_next (0);
}

template <class TYPE> 
void TimerWheel<TYPE>::cancel_i (DHN_Timer_Node_T<TYPE>* n)
{
	this->unlink (n);
	this->free_node (n);
}

template <class TYPE> 
void TimerWheel<TYPE>::recalc_earliest(const TimeValue& last)
{
	// This is possible because we use a count for is_empty()
	if (this->is_empty ())
		return;

	TimeValue et = TimeValue::zero;
	u_int es = 0;
	u_int spoke = this->earliest_spoke_;

	// We will have to go around the wheel at most one time.
	for (u_int i = 0; i < this->spoke_count_; ++i)
	{
		DHN_Timer_Node_T<TYPE>* root = this->spokes_[spoke];
		DHN_Timer_Node_T<TYPE>* n = root->get_next ();
		if (n != root)
		{
			TimeValue t = n->get_timer_value ();
			if (t < last + this->wheel_time_)
			{//This confirms the "last" time value located in this "spoke"
				this->earliest_spoke_ = spoke;
				return;
			}
			else if (et == TimeValue::zero || t < et)
			{
				et = t;
				es = spoke;
			}
		}
		if (++spoke >= this->spoke_count_)
			spoke = 0;
	}

	this->earliest_spoke_ = es;
	//ACE_ERROR((LM_ERROR, "We had to search the whole wheel.\n"));
}

template <class TYPE>
int TimerWheel<TYPE>::cancel_timer(long timer_id,
	const void** act1,
	const void** act2,
	int dont_call_handle_close)
{
	SingleMutex s(TimerQueue<TYPE>::m_mutex);
	DHN_Timer_Node_T<TYPE>* n = this->find_node (timer_id);
	if (n != 0)
	{
		TimeValue last = n->get_timer_value ();

		int recalc = (this->get_first_i () == n);

		if (act1 != 0)
			*act1 = n->get_act1 ();

		if (act2 != 0)
			*act2 = n->get_act2 ();

		this->cancel_i (n);

		if (recalc)
			this->recalc_earliest (last);

		return 1;
	}
	return 0;
}

template <class TYPE> 
unsigned int TimerWheel<TYPE>::calculate_spoke(const TimeValue& t) const
{
	unsigned long ulTime = t.msec();
	unsigned int spoke = static_cast<u_int> ((ulTime >> this->res_bits_) & (this->spoke_count_ - 1));
	return spoke;
}

template <class TYPE> 
long TimerWheel<TYPE>::generate_timer_id (unsigned int spoke)
{
	int cnt_bits = sizeof (long) * 8 - this->spoke_bits_;
	long max_cnt = ((long)1 << cnt_bits) - 1;
	if (spoke == this->spoke_count_)
		--max_cnt; // Because -1 is used as a special invalid timer_id.

	DHN_Timer_Node_T<TYPE>* root = this->spokes_[spoke];

	if (root == root->get_next ())
		root->set_act1(0);

	long next_cnt = reinterpret_cast<long> (root->get_act1 ());

	// This field is used as a counter instead of a timer_id.
	long cnt = root->get_timer_id ();

	if (cnt >= max_cnt && root == root->get_next ())
	{
		// Special case when we overflow on an empty spoke. We can just
		// wrap the count around without searching for duplicates. We only
		// want to do this when the counter overflows, so that we return
		// unique timer_id values as often as possible.
		root->set_timer_id (1);
		return spoke;
	}
	else if (cnt >= max_cnt)
	{ // overflow
		cnt = 0; // try again starting at zero
	}
	else if (next_cnt == 0 || cnt < next_cnt)
	{
		root->set_timer_id (cnt + 1);
		int id =  (cnt << this->spoke_bits_) | spoke;
		return id;
	}

	for (; cnt < max_cnt - 1; ++cnt)
	{
		long id = (cnt << this->spoke_bits_) | spoke;
		DHN_Timer_Node_T<TYPE>* n = this->find_spoke_node (spoke, id);
		if (n == 0)
		{
			root->set_timer_id (cnt + 1);
			// Now we need to find the next highest cnt in use
			next_cnt = 0;
			for (; n != root; n = n->get_next ())
			{
				long tmp = n->get_timer_id () >> this->spoke_bits_;
				if (tmp > cnt && (tmp < next_cnt || next_cnt == 0))
					next_cnt = tmp;
			}

			root->set_act1 (reinterpret_cast<void*> (next_cnt));
			return id;
		}
	}

	return -1; // We did our best, but the spoke is full.
}

template <class TYPE> 
long TimerWheel<TYPE>::schedule_i (const TYPE &type,
	const void *act1,
	const void *act2,
	const TimeValue &future_time,
	const TimeValue &interval)
{
	DHN_Timer_Node_T<TYPE>* n = this->alloc_node ();
	if (n != 0)
	{
		TimeValue cur_time_ = this->gettimeofday();
		//g_logger.Debug("cur time:%I64ds,%ldus,after::%I64ds,%ldus",cur_time_.sec(),cur_time_.usec(),tAfter.sec(),tAfter.usec());
		TimeValue tv = cur_time_ + future_time;
		//g_logger.Debug("schedule time:%I64ds,%ldus",tv.sec(),tv.usec());
		u_int spoke = calculate_spoke (tv);
		long id = generate_timer_id (spoke);

		//ACE_ERROR((LM_ERROR, "Scheduling %x spoke:%d id:%d\n", (long) n, spoke, id));

		if (id != -1)
		{
			n->set (type, act1,act2, tv, interval, 0, 0, id);
			this->schedule_i (n, spoke, tv);
		}
		return id;
	}
	return 0;
}

template <class TYPE> 
void TimerWheel<TYPE>::schedule_i(DHN_Timer_Node_T<TYPE>* n, u_int spoke,const TimeValue& expire)
{
	// See if we need to update the earliest time
	if (this->is_empty() || expire < this->earliest_time ())
	{
		this->earliest_spoke_ = spoke;
	}

	DHN_Timer_Node_T<TYPE>* root = this->spokes_[spoke];
	DHN_Timer_Node_T<TYPE>* last = root->get_prev ();

	++timer_count_;

	// If the spoke is empty
	if (last == root) {
		n->set_prev (root);
		n->set_next (root);
		root->set_prev (n);
		root->set_next (n);
		return;
	}

	DHN_Timer_Node_T<TYPE>* p = root->get_prev ();
	while (p != root && p->get_timer_value () > expire)
		p = p->get_prev ();

	// insert after
	n->set_prev (p);
	n->set_next (p->get_next ());
	p->get_next ()->set_prev (n);
	p->set_next (n);
}

template <class TYPE> 
DHN_Timer_Node_T<TYPE>* TimerWheel<TYPE>::find_spoke_node(unsigned int spoke, long timer_id) const
{
	DHN_Timer_Node_T<TYPE>* root = this->spokes_[spoke];
	for (DHN_Timer_Node_T<TYPE>* n = root->get_next ();
		n != root;
		n = n->get_next ())
	{
		if (n->get_timer_id () == timer_id)
			return n;
	}
	return 0;
}

template <class TYPE>
DHN_Timer_Node_T<TYPE>* TimerWheel<TYPE>::find_node (long timer_id) const
{
	if (timer_id == -1)
		return 0;

	u_int spoke_mask = this->spoke_count_ - 1;
	u_int start = timer_id & spoke_mask;
	DHN_Timer_Node_T<TYPE>* n = this->find_spoke_node (start, timer_id);
	if (n != 0)
		return n;

	// Search the rest of the spokes
	for (u_int i = 0; i < this->spoke_count_; ++i)
	{
		if (i != start)
		{ // already searched this one
			n = this->find_spoke_node (i, timer_id);
			if (n != 0)
				return n;
		}
	}

	return 0;
}

template <class TYPE> 
bool TimerWheel<TYPE>::is_empty (void) const
{
	return timer_count_ == 0;
}

template <class TYPE> 
const TimeValue &TimerWheel<TYPE>::earliest_time (void) const
{
	DHN_Timer_Node_T<TYPE>* n = this->get_first_i ();
	if (n != 0)
	{
		return n->get_timer_value ();
	}
	return TimeValue::zero;
}

template <class TYPE>
DHN_Timer_Node_T<TYPE>* TimerWheel<TYPE>::get_first_i (void) const
{
	DHN_Timer_Node_T<TYPE>* root = this->spokes_[this->earliest_spoke_];
	DHN_Timer_Node_T<TYPE>* first = root->get_next ();
	if (first != root)
	{
		return first;
	}
	return 0;
}

template <class TYPE> 
DHN_Timer_Node_T<TYPE> *TimerWheel<TYPE>::remove_first_expired (const TimeValue& now)
{
	DHN_Timer_Node_T<TYPE>* n = this->get_first ();
	TimeValue tv = n->get_timer_value();
	if (n != 0 && ((tv <= now) || (now.sec() == 0 && now.usec() == 0)))
	{
		this->unlink (n);
		this->recalc_earliest (n->get_timer_value ());
		return n;
	}
	return 0;
}

template <class TYPE> 
DHN_Timer_Node_T<TYPE>* TimerWheel<TYPE>::get_first (void)
{
	return this->get_first_i ();
}

template <class TYPE>
DHN_Timer_Node_T<TYPE> *TimerWheel<TYPE>::remove_first (void)
{
	TimeValue mx(0,0);
	return remove_first_expired (mx);
}

template <class TYPE>
void TimerWheel<TYPE>::reschedule (DHN_Timer_Node_T<TYPE>* n)
{
	const TimeValue& expire = n->get_timer_value ();
	unsigned int spoke = calculate_spoke (expire);
	this->schedule_i (n, spoke, expire);
}
};

// #include "DHNTimerWheel.cpp"

#endif
