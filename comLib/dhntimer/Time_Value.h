#ifndef __DHN_TIME_VALUE_H__
#define __DHN_TIME_VALUE_H__

#include "comLib/sys/gettimeofday.h"
#include "comLib/Logger.h"

long const ONE_SECOND_IN_USECS = 1000000;

namespace DHT
{

class TimeValue
{
public:
	TimeValue (void){}
	explicit TimeValue (time_t sec,long usec = 0){this->set (sec, usec);}
	explicit TimeValue (const struct timeval &t){this->set(t);}

public:
	void set (time_t sec,long usec){this->tv_.tv_sec = sec;this->tv_.tv_usec = usec;normalize();}
	void set (const timeval &tv){this->tv_.tv_sec = tv.tv_sec;this->tv_.tv_usec = tv.tv_usec;normalize();}
	unsigned long msec (void) const;
	void msec (unsigned long long &ms) const;
	void msec (unsigned long long &ms);
	void msec (long);
	void msec (int);
	time_t sec (void) const{return tv_.tv_sec;}
	void sec (time_t sec) {tv_.tv_sec = sec;;}
	long usec (void) const{return this->tv_.tv_usec;}
	void usec (long usec){this->tv_.tv_usec = usec;}
	struct timeval to_timeval();

public:
	TimeValue operator++ (int);
	TimeValue &operator++ (void);
	TimeValue operator-- (int);
	TimeValue &operator-- (void);

	TimeValue &operator += (const TimeValue &tv);
	TimeValue &operator += (time_t tv);
	TimeValue &operator = (const TimeValue &tv);
	TimeValue &operator = (time_t tv);
	TimeValue &operator -= (const TimeValue &tv);
	TimeValue &operator -= (time_t tv);
	static TimeValue gettimeofday (void);

	friend TimeValue operator + (const TimeValue &tv1,const TimeValue &tv2);
	friend TimeValue operator - (const TimeValue &tv1,const TimeValue &tv2);
	friend bool operator < (const TimeValue &tv1,const TimeValue &tv2);
	friend bool operator > (const TimeValue &tv1,const TimeValue &tv2);
	friend bool operator <= (const TimeValue &tv1,const TimeValue &tv2);
	friend bool operator >= (const TimeValue &tv1,const TimeValue &tv2);
	friend bool operator == (const TimeValue &tv1,const TimeValue &tv2);
	friend bool operator != (const TimeValue &tv1,const TimeValue &tv2);
	friend TimeValue operator * (double d,const TimeValue &tv);
	friend TimeValue operator * (const TimeValue &tv,double d);
	

public:
	static const TimeValue zero;
	static const TimeValue max_time;

private:
	void normalize (void);
#ifdef WIN32
	struct 
	{
		time_t tv_sec;
		long tv_usec;
	}tv_;
#else
	timeval tv_;
#endif
};

};

#endif //__DHN_TIME_VALUE_H__
