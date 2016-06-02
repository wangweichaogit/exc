#ifndef __IOS_GENERIC_H__
#define __IOS_GENERIC_H__

#include <mach/mach.h>
#include <mach/semaphore.h>
#include <mach/clock.h>
#include <mach/task.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <setjmp.h>

#ifdef __IOS__

#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 1
static int clock_gettime(int clk_id,struct timespec *ts){
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(),CALENDAR_CLOCK,&cclock);
   	clock_get_time(cclock,&mts);
   	mach_port_deallocate(mach_task_self(),cclock);
	ts->tv_sec = mts.tv_sec;
   	ts->tv_nsec = mts.tv_nsec;
 
	return 0;
}

static int sem_timedwait(semaphore_t *sem,const struct timespec *abs_timeout){
	int ret;
	mach_timespec_t mts;
	mts.tv_sec = abs_timeout->tv_sec;
	mts.tv_nsec = abs_timeout->tv_nsec;
	ret = semaphore_timedwait(*sem,mts);
	switch (ret){
	case KERN_SUCCESS:
		return 0;
	case KERN_OPERATION_TIMED_OUT:
		errno = ETIMEDOUT;
		break;
	case KERN_ABORTED:
		errno = EINTR;
		break;
	default:
		errno = EINVAL;
		break;
	}

	return -1;
}
#endif

static int _vsnprintf(char *buffer,size_t count,const char *format,va_list argptr){
	int ret = -1;
#ifdef WIN32
	ret = ::_vsnprintf(buffer,count,format,argptr);
#else
	ret = vsnprintf(buffer,count,format,argptr);
#endif
	return ret;
}

#ifndef WIN32
//typedef size_t errno_t;
#endif
static errno_t strncpy_s(char *dest,const char *src,size_t count){
#ifdef WIN32
	return ::strncpy_s(dest,src,count);
#elif defined(__IOS__)
	return strlcpy(dest,src,count);
#endif
}
static errno_t strcpy_s(char *dest,const char *src){
#ifdef WIN32
	return ::strcpy_s(dest,src);
#elif defined(__IOS__)
	return (size_t)strcpy(dest,src);
#endif
}

static void Sleep(unsigned long delay){
#ifdef WIN32
        ::Sleep(delay);
#else
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = (delay % 1000) * 1000 * 1000;
        nanosleep(&ts,NULL);
#endif
}

#ifdef WIN32
static DWORD  WaitForSingleObject(HANDLE hHandle,DWORD dwMilliseconds){
	return ::WaitForSingleObject(hHandle,dwMilliseconds);
#elif defined __IOS__
typedef unsigned long DWORD;
#define WAIT_TIMEOUT             258L
#define STATUS_WAIT_0            ((DWORD)0x00000000L)
#define WAIT_OBJECT_0            ((STATUS_WAIT_0)+0)
#define STATUS_ABANDONED_WAIT_0  ((DWORD)0x00000080L)
#define WAIT_ABANDONED           ((STATUS_ABANDONED_WAIT_0)+0)
static DWORD  WaitForSingleObject(semaphore_t sem,DWORD dwMilliseconds){
	struct timespec ts;
	ts.tv_sec = dwMilliseconds / 1000;
	ts.tv_nsec = (dwMilliseconds % 1000) * 1000000;
	if (sem_timedwait(&sem,&ts) == -1){
		switch (errno){
		case ETIMEDOUT:
			return WAIT_TIMEOUT;
		case EINTR:
			return WAIT_ABANDONED;
		default:
			return EINVAL;
		}
	}
	return WAIT_OBJECT_0;	
#endif
}

#endif //__IOS_GENERIC_H__
