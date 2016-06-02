#ifndef __TIME_VALUE_CPP__
#define __TIME_VALUE_CPP__


#include "comLib/dhntimer/Time_Value.h"
#include "comLib/sys/gettimeofday.h"

namespace DHT
{
const TimeValue TimeValue::zero(0,0);
const TimeValue TimeValue::max_time (0x0FFFFFFF,0x0FFFFFFF);

unsigned long
TimeValue::msec (void) const
{
	time_t secs = this->tv_.tv_sec * 1000 + this->tv_.tv_usec / 1000;
	//return ACE_Utils::truncate_cast<unsigned long> (secs);
	return (unsigned long)secs;
}

void TimeValue::msec (unsigned long long &ms) const
{
	// ACE_OS_TRACE ("ACE_Time_Value::msec");
	ms = (unsigned long long)this->tv_.tv_sec;
	ms *= 1000;
	ms += (this->tv_.tv_usec / 1000);
}

void TimeValue::msec (unsigned long long &ms) /*const*/
{
	// ACE_OS_TRACE ("ACE_Time_Value::msec");
	const TimeValue *tv = this;
	tv->msec (ms);
}

void TimeValue::msec (long milliseconds)
{
	// ACE_OS_TRACE ("ACE_Time_Value::msec");
	// Convert millisecond units to seconds;
	long secs = milliseconds / 1000;
	this->tv_.tv_sec = secs;
	// Convert remainder to microseconds;
	this->tv_.tv_usec = (milliseconds - (secs * 1000)) * 1000;
}

/// Converts from milli-seconds format into Time_Value format.
void TimeValue::msec (int milliseconds)
{
	TimeValue::msec (static_cast<long> (milliseconds));
}

bool operator > (const TimeValue &tv1,const TimeValue &tv2)
{
	if (tv1.sec () > tv2.sec ())
		return 1;
	else if (tv1.sec () == tv2.sec ()
		&& tv1.usec () > tv2.usec ())
		return 1;
	else
		return 0;
}

bool operator >= (const TimeValue &tv1,const TimeValue &tv2)
{
	if (tv1.sec () > tv2.sec ())
		return 1;
	else if (tv1.sec () == tv2.sec ()
		&& tv1.usec () >= tv2.usec ())
		return 1;
	else
		return 0;
}

bool operator < (const TimeValue &tv1,const TimeValue &tv2)
{
	return tv2 > tv1;
}

bool operator <= (const TimeValue &tv1,const TimeValue &tv2)
{
	return tv2 >= tv1;
}

bool operator == (const TimeValue &tv1,
			 const TimeValue &tv2)
{
	return tv1.sec () == tv2.sec ()
		&& tv1.usec () == tv2.usec ();
}

bool operator != (const TimeValue &tv1,
			 const TimeValue &tv2)
{
	// ACE_OS_TRACE ("operator !=");
	return !(tv1 == tv2);
}

TimeValue TimeValue::operator ++ (int)
{
  TimeValue tv (*this);
  ++*this;
  return tv;
}

TimeValue &TimeValue::operator ++ (void)
{
  this->usec (this->usec () + 1);
  this->normalize ();
  return *this;
}

TimeValue TimeValue::operator -- (int)
{
  TimeValue tv (*this);
  --*this;
  return tv;
}

TimeValue & TimeValue::operator -- (void)
{
  this->usec (this->usec () - 1);
  this->normalize ();
  return *this;
}

TimeValue &TimeValue::operator+= (const TimeValue &tv)
{
	this->sec (this->sec () + tv.sec ());
	this->usec (this->usec () + tv.usec ());
	this->normalize ();
	return *this;
}

TimeValue &TimeValue::operator+= (time_t tv)
{
	this->sec (this->sec () + tv);
	return *this;
}

TimeValue &TimeValue::operator= (const TimeValue &tv)
{
	this->sec (tv.sec ());
	this->usec (tv.usec ());
	return *this;
}

TimeValue &TimeValue::operator= (time_t tv)
{
	this->sec (tv);
	this->usec (0);
	return *this;
}

TimeValue &TimeValue::operator-= (const TimeValue &tv)
{
	this->sec (this->sec () - tv.sec ());
	this->usec (this->usec () - tv.usec ());
	this->normalize ();
	return *this;
}

TimeValue &TimeValue::operator-= (time_t tv)
{
	this->sec (this->sec () - tv);
	return *this;
}

TimeValue operator + (const TimeValue &tv1,const TimeValue &tv2)
{
	TimeValue sum (tv1);
	sum += tv2;

	return sum;
}

TimeValue operator - (const TimeValue &tv1,const TimeValue &tv2)
{
	TimeValue delta (tv1);
	delta -= tv2;

	return delta;
}

void TimeValue::normalize (void)
{
	if (this->tv_.tv_usec >= ONE_SECOND_IN_USECS)
	{
		do
		{
		  ++this->tv_.tv_sec;
		  this->tv_.tv_usec -= ONE_SECOND_IN_USECS;
		}
		while (this->tv_.tv_usec >= ONE_SECOND_IN_USECS);
    }
	else if (this->tv_.tv_usec <= -ONE_SECOND_IN_USECS)
    {
		do
        {
          --this->tv_.tv_sec;
          this->tv_.tv_usec += ONE_SECOND_IN_USECS;
        }
		while (this->tv_.tv_usec <= -ONE_SECOND_IN_USECS);
    }
	
	if (this->tv_.tv_sec >= 1 && this->tv_.tv_usec < 0)
    {
		--this->tv_.tv_sec;
		this->tv_.tv_usec += ONE_SECOND_IN_USECS;
    }
	else if (this->tv_.tv_sec < 0 && this->tv_.tv_usec > 0)
    {
		++this->tv_.tv_sec;
		this->tv_.tv_usec -= ONE_SECOND_IN_USECS;
    }
}

TimeValue TimeValue::gettimeofday (void)
{
	struct timeval tv;
	DHT::gettimeofday_relative(&tv,NULL);
	TimeValue t(tv);
	return t;
}

struct timeval TimeValue::to_timeval()
{
	struct timeval tv;
	memset(&tv,0,sizeof(tv));
	tv.tv_usec = tv.tv_usec;
	tv.tv_sec = (long)tv_.tv_sec;
	return tv;
}


};

#endif
