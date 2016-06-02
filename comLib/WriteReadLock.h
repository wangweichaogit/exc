#ifndef _DHT_CODE_COMLIB_WRITELOCK_H_
#define _DHT_CODE_COMLIB_WRITELOCK_H_
namespace DHT
{
#include "comLib/ReadWriteLock.h"
	typedef ReadWriteLock WriteReadLock;
};
//class WriteReadLock
//{
//public:
//    WriteReadLock()
//    {
//#ifdef _LINUX_
//        pthread_rwlock_init(&rwlock,NULL);
//#endif
//    }
//
//    ~WriteReadLock()
//    {
//#ifdef _LINUX_
//        pthread_rwlock_destroy(&rwlock);
//#endif
//    }
//
//    int WriteLock()
//    {
//        int nRet = 0;
//#ifdef _LINUX_
//        nRet = pthread_rwlock_wrlock(&rwlock);
//#endif
//        return nRet;
//    }
//
//    int ReadLock()
//    {
//        int nRet = 0;
//#ifdef _LINUX_
//        pthread_rwlock_rdlock(&rwlock);
//#endif
//        return nRet;
//    }
//
//    int UnLock()
//    {
//        int nRet = 0;
//#ifdef _LINUX_
//        nRet = pthread_rwlock_unlock(&rwlock);
//#endif
//        return nRet;
//    }
//private:
//#ifdef _LINUX_
//    pthread_rwlock_t rwlock;
//#endif
//};

#endif