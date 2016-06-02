/*
 * Copyright (C) 2008 The Trustees of Columbia University in the City of New York. 
 * All rights reserved.
 *
 * See the file LICENSE included in this distribution for details.
 */

#include "gettimeofday.h"
#include <stdio.h>
#ifdef __IOS__
#include "ios_generic.h"
#endif

namespace DHT
{

#ifdef WIN32

static int64 filetime_to_unix_epoch (const FILETIME *ft)
{
	int64 res = (int64) ft->dwHighDateTime << 32;

	res |= ft->dwLowDateTime;
	res /= 10;                   //from 100 nano-sec periods to usec
	res -= DELTA_EPOCH_IN_USEC;  // from Win epoch to Unix epoch 
	return (res);
}

int gettimeofday_absolute (struct timeval *tv, void *tz)
{
	FILETIME  ft;
	int64 tim;

	if (!tv) {
		errno = EINVAL;
		return (-1);
	}
	GetSystemTimeAsFileTime (&ft);
	tim = filetime_to_unix_epoch (&ft);
	tv->tv_sec  = (long) (tim / 1000000L);
	tv->tv_usec = (long) (tim % 1000000L);
	return (0);
}

int gettimeofday_relative (struct timeval *tv, void *tz)
{
	LARGE_INTEGER freq,counter;
	int64 nCurTime;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&counter);  

	nCurTime	= 1000000L * counter.QuadPart / freq.QuadPart;
	tv->tv_sec  = (long) (nCurTime / 1000000L);
	tv->tv_usec = (long) (nCurTime % 1000000L);
	return 0;
}
#else
#include <time.h>
int gettimeofday_relative (struct timeval *tv, void *tz)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC,&ts);
	tv->tv_sec	= ts.tv_sec;
	tv->tv_usec	= ts.tv_nsec/1000;
	return 0;
}

int gettimeofday_absolute (struct timeval *tv, void *tz)
{
	return ::gettimeofday(tv,(struct timezone *)tz);
}
#endif


struct timeval difftimeval(const struct timeval& after, const struct timeval& before) {

  struct timeval diff;
  //long int t1;

  diff.tv_sec = diff.tv_usec = 0;
  
  if(after.tv_sec > before.tv_sec)
  {
    if(after.tv_usec >= before.tv_usec) 
	{

      diff.tv_sec = after.tv_sec - before.tv_sec;
      diff.tv_usec = after.tv_usec - before.tv_usec;

    } 
	else 
	{

      /*
      t1=10;
      while( (unsigned int)(after.tv_usec%t1) != (unsigned int)after.tv_usec) {
	t1=t1*10;
      }
      while( (unsigned int)(t1+after.tv_usec) < (unsigned int)before.tv_usec) {
	t1=t1*10;
      }
      */
      
      /*
      while( (t1+after.tv_usec) < before.tv_usec) { 
	t1=t1*10;
      }
      */

      diff.tv_sec = after.tv_sec - before.tv_sec - 1;
      //t1 = t1 + after.tv_usec;
      diff.tv_usec = 1000000 + after.tv_usec - before.tv_usec;

      //TRACE(5, " after sec: %u usec: %lu before sec: %u usec: %lu\n", after.tv_sec, after.tv_usec, before.tv_sec, before.tv_usec);
    }
    
  } 
  else if(after.tv_sec == before.tv_sec) 
  {
    if(after.tv_usec > before.tv_usec) 
	{
      diff.tv_usec = after.tv_usec - before.tv_usec;
    }
  }
  
  //diff_sec = diff.tv_sec;
  //diff_usec = diff.tv_usec%1000000;
  
  diff.tv_usec = diff.tv_usec%1000000;

  return diff;
}//difftime



struct timeval addtimeval(struct timeval calc_time, int sec, int usec) 
{
  
  int t1;

  t1 = (calc_time.tv_usec + usec) / 1000000; //divide
  calc_time.tv_usec = (calc_time.tv_usec + usec) % 1000000;

  calc_time.tv_sec = calc_time.tv_sec + sec + t1;
  
  return calc_time;

}//addtimeval

void printlocaltime( FILE *fp) {

  if(!fp)
    return;

#ifdef WIN32
  SYSTEMTIME st;
  
  GetLocalTime(&st);
  fprintf(fp, "%d/%d/%d %d:%d:%d\n", st.wMonth, 
	 st.wDay, st.wYear, st.wHour, st.wMinute, st.wSecond ); 

#else

  struct tm *local_time;
  struct timeval tv;

  gettimeofday_absolute(&tv, NULL);

  local_time = localtime((time_t*)&tv.tv_sec);
  fprintf(fp, "%d/%d/%d %d:%d:%d\n", local_time->tm_mon+1, 
	 local_time->tm_mday, local_time->tm_year+1900, 
	 local_time->tm_hour, local_time->tm_min, 
	 local_time->tm_sec);

#endif

}

};
