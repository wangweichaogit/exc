//
//  LogHelper.cpp
//  LogHelper
//
//  Created by gongai on 14-10-27.
//  Copyright 2014 青牛(北京)技术有限公司. All rights reserved.
//

#include "comLib/Logger.h"
#include <stdarg.h>

// --------------------------------------------------------------------------
class LogHelper
{
public:
    static LogHelper* Instance();
    static void ExitInstance();
    
    void InitLogCfg(const char* logpath, const char* filename, int loglevel);
    void UninitLog();
    
    DHT::Logger* m_applog;
    DHT::LogManager* m_log_manager;
    
private:
    static LogHelper* m_singleton;
    LogHelper();
    ~LogHelper();
};

// --------------------------------------------------------------------------
LogHelper* LogHelper::m_singleton = NULL;

LogHelper::LogHelper()
{
    m_applog = NULL;
    m_log_manager = NULL;
}

LogHelper::~LogHelper()
{
}

LogHelper* LogHelper::Instance()
{
    if (m_singleton == NULL) {
        m_singleton = new LogHelper();
    }
    
    return m_singleton;
}

void LogHelper::ExitInstance()
{
    if (m_singleton) {
        delete m_singleton;
        m_singleton = NULL;
    }
}

void LogHelper::InitLogCfg(const char* logpath, const char* filename, int loglevel)
{
    if (logpath == NULL || strlen(logpath) == 0) {
        printf("parameter logpath is empty");
        return;
    }
    
    if (filename == NULL || strlen(filename) == 0) {
        printf("parameter filename is empty");
        return;
    }
    
    if (loglevel < 0 || loglevel > 9) {
        printf("parameter loglevel must be in [0-9]");
        return;
    }
    
    m_log_manager = new DHT::LogManager();
    m_log_manager->CreateLongDirectory(std::string(logpath));
    m_log_manager->SetLogFilePath(logpath);
    m_log_manager->SetLogLevel(loglevel);    
    m_log_manager->Open(filename, true, true, true, 2048, 0, 0);
    
    m_applog = new DHT::Logger("NetPhone", m_log_manager);
}

void LogHelper::UninitLog()
{
    if (m_applog != NULL)
    {
        delete m_applog;
        m_applog = NULL;
    }
    
    if (m_log_manager)
    {
        m_log_manager->Close();
        delete m_log_manager;
        m_log_manager = NULL;
    }
}

// --------------------------------------------------------------------------
#define LOG_BUFFER_SIZE  1024

#ifdef __cplusplus
    extern "C" {
#endif

void COMLIBAPI InitLogCfg(const char* logdir,const char* filename, int loglevel)
{
    if (logdir == NULL || strlen(logdir) == 0) {
        printf("parameter logdir is empty");
        return;
    }
    
    LogHelper::Instance()->InitLogCfg(logdir, filename, loglevel);
}

void COMLIBAPI UninitLogCfg()
{
    LogHelper::Instance()->UninitLog();
    
    LogHelper::ExitInstance();
}

void COMLIBAPI WriteLogD(const char* format, ...)
{
    try {
        char szBuff[LOG_BUFFER_SIZE] = {0};
        
        va_list va;
        va_start(va, format);
        vsnprintf(szBuff, LOG_BUFFER_SIZE, format, va);
        va_end(va);
        
        if (LogHelper::Instance()->m_applog) {
            LogHelper::Instance()->m_applog->Append(DHT::Logger::LL_DEBUG, szBuff);
        }
    } catch (...) {
    }
}

void COMLIBAPI WriteLogE(const char* format, ...)
{
    try {
        char szBuff[LOG_BUFFER_SIZE] = {0};
        
        va_list va;
        va_start(va, format);
        vsnprintf(szBuff, LOG_BUFFER_SIZE, format, va);
        va_end(va);
        
        if (LogHelper::Instance()->m_applog) {
            LogHelper::Instance()->m_applog->Append(DHT::Logger::LL_ERROR, szBuff);
        }
    } catch (...) {
    }
}

void COMLIBAPI WriteLogW(const char* format, ...)
{
    try {
        char szBuff[LOG_BUFFER_SIZE] = {0};
        
        va_list va;
        va_start(va, format);
        vsnprintf(szBuff, LOG_BUFFER_SIZE, format, va);
        va_end(va);
        
        if (LogHelper::Instance()->m_applog) {
            LogHelper::Instance()->m_applog->Append(DHT::Logger::LL_WARN, szBuff);
        }
    } catch (...) {
    }
}

void COMLIBAPI WriteLogI(const char* format, ...)
{
    try {
        char szBuff[LOG_BUFFER_SIZE] = {0};
        
        va_list va;
        va_start(va, format);
        vsnprintf(szBuff, LOG_BUFFER_SIZE, format, va);
        va_end(va);
        
        if (LogHelper::Instance()->m_applog) {
            LogHelper::Instance()->m_applog->Append(DHT::Logger::LL_INFO, szBuff);
        }
    } catch (...) {
    }
}

#ifdef __cplusplus
    }
#endif