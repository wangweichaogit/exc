// Logger_type.h: interface for the Logger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGGER_H__0C0D5FA0_DAC4_4BB8_B2A7_EFD14C996464__INCLUDED_)
#define AFX_LOGGER_H__0C0D5FA0_DAC4_4BB8_B2A7_EFD14C996464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#ifdef WIN32
#include <cstring>
#endif
#include <string.h>

namespace DHT
{

typedef void (*PrintLogEntryCallback)(void *context,int nLogLevel,const char *szFormattedText);

typedef enum LogMode
{
    NOT_WRITE_LOG = 0,
    ONLY_WRITE_LOG = 1,
    APP_WRITE_LOG = 2,
    BOTH_WRITE_LOG = 3
}LogMode;

typedef struct LogConfigure
{
    LogConfigure()
    {
        log_mode = ONLY_WRITE_LOG;
        log_level = 0;
        log_context = NULL;
        pfnLogCallback = NULL;
        memset(log_path,0,sizeof(log_path));
        roll_volumes = 0;
        volume_size = 1024 * 10; //10M
        memset(prefix,0,sizeof(prefix));
    }

    LogMode log_mode;
    int log_level;
    void *log_context;
    PrintLogEntryCallback pfnLogCallback;
    char log_path[256];
    int roll_volumes;
    int volume_size;
    char prefix[64];
}LogConfigure;
    
}

#endif // !defined(AFX_LOGGER_H__0C0D5FA0_DAC4_4BB8_B2A7_EFD14C996464__INCLUDED_)
