#ifndef __DHT_READ_WRITE_LOCK_H__
#define __DHT_READ_WRITE_LOCK_H__

#ifndef _LINUX_
#include "GMRWLockImp.h"
#endif
#include "LogHelper.h"
#include "GMRWLockImp.h"

namespace DHT
{

class COMLIBAPI ReadWriteLock
{
public:
    ReadWriteLock()
    {
#if defined _LINUX_ && !defined _ANDROID_
        pthread_rwlock_init(&rwlock,NULL);
#endif
    }

    ~ReadWriteLock()
    {
#if defined _LINUX_ && !defined _ANDROID_
        pthread_rwlock_destroy(&rwlock);
#endif
    }

    int WriteLock()
    {
        int nRet = 0;
#if defined _LINUX_ && !defined _ANDROID_
        nRet = pthread_rwlock_wrlock(&rwlock);
#else
		wrlock.writeLock();
#endif
        return nRet;
    }

    int ReadLock()
    {
        int nRet = 0;
#if defined _LINUX_ && !defined _ANDROID_
        pthread_rwlock_rdlock(&rwlock);
#else
		wrlock.readLock();
#endif
        return nRet;
    }

    int UnLock()
    {
        int nRet = 0;
#if defined _LINUX_ && !defined _ANDROID_
        nRet = pthread_rwlock_unlock(&rwlock);
#else
		wrlock.unlock();
#endif
        return nRet;
    }
private:
#if defined _LINUX_ && !defined _ANDROID_
	pthread_rwlock_t rwlock;
#else
	GMJRRWLock	wrlock;
#endif
};


} // namespace DHT

#endif // __DHT_READ_WRITE_LOCK_H__

