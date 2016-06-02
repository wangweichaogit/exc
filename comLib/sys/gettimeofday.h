/*
 * Copyright (C) 2008 The Trustees of Columbia University in the City of New York. 
 * All rights reserved.
 *
 * See the file LICENSE included in this distribution for details.
 */

#ifndef _GETTIMEOFDAY_H
#define _GETTIMEOFDAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#ifdef WIN32
#define EINVAL 22
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS) || defined(__WATCOMC__)
#define DELTA_EPOCH_IN_USEC  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_USEC  11644473600000000ULL
#endif
#include <WinSock2.h>
#else
#include <sys/time.h> 
#endif

#include "comLib/Stdafx.h"

namespace DHT
{

COMLIBAPI int gettimeofday_relative (struct timeval *tv, void *tz);
#ifdef WIN32
typedef unsigned __int64 int64;
#endif

COMLIBAPI int gettimeofday_absolute (struct timeval *tv, void *tz);
COMLIBAPI struct timeval difftimeval(const struct timeval& after, const struct timeval& before) ;
COMLIBAPI struct timeval addtimeval(struct timeval calc_time, int sec, int usec);

void printlocaltime( FILE *fp);

};

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif //#ifndef _GETTIMEOFDAY_H

