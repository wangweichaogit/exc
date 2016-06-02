// Logger.cpp: implementation of the Logger class.
//
//////////////////////////////////////////////////////////////////////

#include "comLib/Logger.h"
#include "comLib/Logger_impl.h"

#include <stdio.h>
#include <stdarg.h>
#include <sys/timeb.h>

#ifdef WIN32
#   include <time.h>
#elif defined(_REDHAT_) || defined(__IOS__) ||defined _LINUX_
#   include <sys/syscall.h>
typedef unsigned long DWORD;
#else
#   include <sys/types.h>
#   include <linux/unistd.h>
#   include <errno.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace DHT
{

#ifndef _WINDOWS_
#define _vsnprintf(s1, n, s2, v)  vsnprintf(s1, n, s2, v)
#endif

/////////////////////////////////////LogEvent/////////////////////////////////////
LogEvent::LogEvent(LOG_EVENT_TYPE eType, const std::string& strLine)
    : m_eType(eType), m_strLine(strLine)
{
    
}

LogEvent::~LogEvent()
{

}

////////////////////////////////////Logger//////////////////////////////////////
Logger::Logger(LogManager* log_manager)
{
	memset(m_strTag, 0, sizeof(m_strTag));
	if (NULL != log_manager)
		m_log_impl = log_manager->m_log_impl;
	else
		m_log_impl = NULL;
}

Logger::Logger(const char* pszTag, LogManager* log_manager)
{
	memset(m_strTag, 0, sizeof(m_strTag));
	if (NULL != pszTag)
		strncpy(m_strTag, pszTag, sizeof(m_strTag)-1);
    
	if (NULL != log_manager)
		m_log_impl = log_manager->m_log_impl;
	else
		m_log_impl = NULL;
}

Logger::~Logger()
{

}
    
void Logger::Init(const char* tag, LogManager* log_manager)
{
    memset(m_strTag, 0, sizeof(m_strTag));
    strncpy(m_strTag, tag, sizeof(m_strTag)-1);
    
    m_log_impl = log_manager->m_log_impl;
}
    
void Logger::SetTag(const char* format, ...)
{
	memset(m_strTag, 0, sizeof(m_strTag));	
	try
	{
		va_list args;
		va_start(args, format);		
		/*int nBuf = */_vsnprintf(m_strTag, sizeof(m_strTag)-1, format, args);
		va_end(args);
	}catch(...){}
}
    
#define GENERAL_LOG_FORMAT(X)	 \
    std::string buf = ""; \
    if (!m_log_impl->IsAppWriteLog()) { \
        LogLayout lot; \
        buf = lot.GetLayoutString(X, m_strTag); \
    } \
    int nBuf = 0; \
    char szBuffer[2048] = {0}; \
    try { \
        va_list args; \
        va_start(args, format); \
        nBuf = _vsnprintf(szBuffer, 2048, format, args); \
        va_end(args); \
    }catch(...){ \
    } \
    szBuffer[2047] = '\0'; \
    buf.append(szBuffer); \
    try { \
        m_log_impl->AppendLine(buf); \
    }catch(...){ \
    } \

void Logger::Debug(const char* format, ...)
{
	if (false == isDebugEnabled()) return;
    if (NULL == m_log_impl) {
        LogLayout lot;
        std::string prefix = lot.GetLayoutString(LL_DEBUG, m_strTag);
        printf("%s m_log_impl is null\n",prefix.c_str());
        return;
    }
    GENERAL_LOG_FORMAT(LL_DEBUG);
}

void Logger::Info(const char* format, ...)
{
	if (false == isInfoEnabled()) return;
    if (NULL == m_log_impl) {
        LogLayout lot;
        std::string prefix = lot.GetLayoutString(LL_INFO, m_strTag);
        printf("%s m_log_impl is null\n",prefix.c_str());
        return;
    }
	GENERAL_LOG_FORMAT(LL_INFO);
}

void Logger::Trace(const char* format, ...)
{
	if (false == isTraceEnabled()) return;
    if (NULL == m_log_impl) {
        LogLayout lot;
        std::string prefix = lot.GetLayoutString(LL_TRACE, m_strTag);
        printf("%s m_log_impl is null\n",prefix.c_str());
        return;
    }
	GENERAL_LOG_FORMAT(LL_TRACE);
}

void Logger::Warn(const char* format, ...)
{
	if (false == isWarnEnabled()) return;
    if (NULL == m_log_impl) {
        LogLayout lot;
        std::string prefix = lot.GetLayoutString(LL_WARN, m_strTag);
        printf("%s m_log_impl is null\n",prefix.c_str());
        return;
    }
	GENERAL_LOG_FORMAT(LL_WARN);
}

void Logger::Error(const char* format, ...)
{
	if (false == isErrorEnabled()) return;
    if (NULL == m_log_impl) {
        LogLayout lot;
        std::string prefix = lot.GetLayoutString(LL_ERROR, m_strTag);
        printf("%s m_log_impl is null\n",prefix.c_str());
        return;
    }
	GENERAL_LOG_FORMAT(LL_ERROR);
}

void Logger::Fatal(const char* format, ...)
{
	if (false == isFatalEnabled()) return;
    if (NULL == m_log_impl) {
        LogLayout lot;
        std::string prefix = lot.GetLayoutString(LL_FATAL, m_strTag);
        printf("%s m_log_impl is null\n",prefix.c_str());
        return;
    }
	GENERAL_LOG_FORMAT(LL_FATAL);
}

void Logger::Write(const char* format, ...)
{
    if (NULL == m_log_impl) {
        LogLayout lot;
        std::string prefix = lot.GetLayoutString(LL_INFO, m_strTag);
        printf("%s m_log_impl is null\n",prefix.c_str());
        return;
    }
	GENERAL_LOG_FORMAT(LL_INFO);
}

void Logger::Append(const char* msg)
{
	if (false == isDebugEnabled() || !msg) return;
	
    LogLayout lot;
    std::string buf = lot.GetLayoutString(LL_DEBUG, m_strTag);
	buf.append(msg);

	if (NULL != m_log_impl)
		m_log_impl->AppendLine(buf);
}
    
void Logger::Append(int level, const char* msg)
{
    if (level ==LL_DEBUG)
        if (false == isDebugEnabled())
            return;
        
    if (level ==LL_INFO)
        if (false == isInfoEnabled())
            return;
            
    if (level ==LL_WARN)
        if (false == isWarnEnabled())
            return;
            
    if (level ==LL_ERROR)
        if (false == isErrorEnabled())
            return;

	if (!msg)
		return;
	
	int nlen = strlen(msg);
	if (nlen >= 2047)
		return;

    LogLayout lot;
    std::string buf = lot.GetLayoutString(level, m_strTag);
    buf.append(msg);
    
	if (NULL != m_log_impl)
		m_log_impl->AppendLine(buf);
}

bool Logger::isDebugEnabled()
{
	if (this && NULL != m_log_impl)
		return (m_log_impl->GetLogLevel()>=LL_DEBUG);
	else
		return false;
}

bool Logger::isInfoEnabled()
{
	if (NULL != m_log_impl)
		return (m_log_impl->GetLogLevel()>=LL_INFO);
	else
		return false;
}

bool Logger::isTraceEnabled()
{
	if (NULL != m_log_impl)
		return (m_log_impl->GetLogLevel()>=LL_TRACE);
	else
		return false;
}

bool Logger::isWarnEnabled()
{
	if (NULL != m_log_impl)
		return (m_log_impl->GetLogLevel()>=LL_WARN);
	else
		return false;
}

bool Logger::isErrorEnabled()
{
	if (NULL != m_log_impl)
		return (m_log_impl->GetLogLevel()>=LL_ERROR);
	else
		return false;
}

bool Logger::isFatalEnabled()
{
	if (NULL != m_log_impl)
		return (m_log_impl->GetLogLevel()>=LL_FATAL);
	else
		return false;
}

void Logger::Debug(LogLayout& lot )
{
	if (false == isDebugEnabled()) return;
    std::string buf = lot.GetLayoutString(LL_DEBUG, m_strTag);
	if (NULL != m_log_impl)
		m_log_impl->AppendLine(buf);

}

void Logger::Info(LogLayout& lot)
{
	if (false == isInfoEnabled()) return;
	std::string buf = lot.GetLayoutString(LL_INFO, m_strTag);
	if (NULL != m_log_impl)
		m_log_impl->AppendLine(buf);
}

void Logger::Trace(LogLayout& lot)
{
	if (false == isTraceEnabled()) return;
	std::string buf = lot.GetLayoutString(LL_TRACE, m_strTag);
	if (NULL != m_log_impl)
		m_log_impl->AppendLine(buf);
}

void Logger::Warn(LogLayout& lot)
{
	if (false == isWarnEnabled()) return;
	std::string buf = lot.GetLayoutString(LL_WARN, m_strTag);
	if (NULL != m_log_impl)
		m_log_impl->AppendLine(buf);
}

void Logger::Error(LogLayout& lot)
{
	if (false == isErrorEnabled()) return;
	std::string buf = lot.GetLayoutString(LL_ERROR, m_strTag);
	if (NULL != m_log_impl)
		m_log_impl->AppendLine(buf);
}

void Logger::Fatal(LogLayout& lot)
{
	if (false == isFatalEnabled()) return;
	std::string buf = lot.GetLayoutString(LL_FATAL, m_strTag);
	if (NULL != m_log_impl)
		m_log_impl->AppendLine(buf);
}

void Logger::Write(LogLayout& lot)
{
	std::string buf = lot.GetLayoutString(LL_ALL, m_strTag);
	if (NULL != m_log_impl)
		m_log_impl->AppendLine(buf);
}
    
std::string LogLayout::GetLevelString(int nLevel2Str)
{
	switch(nLevel2Str)
	{
	case Logger::LL_FATAL: return "FR";
	case Logger::LL_ERROR: return "ER";
	case Logger::LL_WARN:  return "WA";
	case Logger::LL_TRACE: return "TR";
	case Logger::LL_INFO:  return "IF";
	case Logger::LL_DEBUG: return "DT";
	case Logger::LL_ALL: return "**";
	default: return "--";
	}
}
    
LogLayout::LogLayout()
{
	m_strBuf="";
}
LogLayout::LogLayout(char* str1)
{
	m_strBuf.append("[").append(str1).append("][][]");
}
LogLayout::LogLayout(char* str1, char* str2)
{
	m_strBuf.append("[").append(str1).append("]");
	m_strBuf.append("[").append(str2).append("][]");
}
LogLayout::LogLayout(char* str1, char* str2, char* str3)
{
	m_strBuf.append("[").append(str1).append("]");
	m_strBuf.append("[").append(str2).append("]");
	m_strBuf.append("[").append(str3).append("]");
}
void LogLayout::AppendLine(const char* format, ...)
{
	int nBuf;
	char szBuffer[512];
	memset(szBuffer, 0, sizeof(szBuffer));
	
	try
	{
		va_list args;
		va_start(args, format);
		
#ifdef _WINDOWS_
		nBuf = _vsnprintf(szBuffer, 512, format, args);
#else		// no windows
		nBuf = vsnprintf(szBuffer, 512, format, args);
#endif
		va_end(args);
	}catch(...){}
	szBuffer[511]=0;
	//m_strMessage.append(szBuffer);
	m_messages.push_back(szBuffer);
}

//time-LV-[threadid][Tag][str1][str2][str3]
std::string  LogLayout::GetLayoutString(int nLevel2Str, std::string strClassTag)
{
	char   strFix[256];
	memset(strFix, 0, sizeof(strFix));
	//GetCurrentThreadID();
	int sid = 0;
    DWORD tid = 0;
#ifdef WIN32
	tid = GetCurrentThreadId();
#else
	pthread_t stid = pthread_self();
    tid = (DWORD)stid;
#if (defined(_REDHAT_) || defined(__IOS__) || defined _LINUX_) && !defined _ANDROID_
	sid = syscall(SYS_gettid);
#else
	sid = gettid();
#endif
#if defined(__IOS__)
    // http://stackoverflow.com/questions/8995650/what-does-the-prefix-in-nslog-mean
    __uint64_t threadId;
    if (pthread_threadid_np(stid, &threadId)) {
        tid= pthread_mach_thread_np(stid);
    } else {
        tid = (DWORD)threadId;
    }
#else
    tid=(DWORD)stid;
#endif
    
#ifndef _AIX
//	tid = tid<<8;
//	tid = tid>>20;
#endif //_AIX

#endif
	if ( m_strBuf.size()<=0 )
	{
    #ifdef WIN32
		_snprintf(strFix, 256, "[%s][%s]-[%lu(%d)][%s]",
			GetLevelString(nLevel2Str).c_str(), NowStringNoYear().c_str(), tid,sid, strClassTag.c_str());
    #else
        snprintf(strFix, 256, "[%s][%s]-[%lu(%d)][%s]",
                  GetLevelString(nLevel2Str).c_str(), NowStringNoYear().c_str(), tid,sid, strClassTag.c_str());
    #endif
	}else{
    #ifdef WIN32
		_snprintf(strFix, 256, "[%s][%s]-[%lu(%d)][%s]%s",
            GetLevelString(nLevel2Str).c_str(), NowStringNoYear().c_str(), tid,sid, strClassTag.c_str(), m_strBuf.c_str());
    #else
        snprintf(strFix, 256, "[%s][%s]-[%lu(%d)][%s]",
                 GetLevelString(nLevel2Str).c_str(), NowStringNoYear().c_str(), tid,sid, strClassTag.c_str());
    #endif
	}
	std::string buf;
	
	if ( m_messages.size()<=0)
	{
		buf.append(strFix);
	}
	else
	{
		typedef std::vector<std::string>::iterator itVector;
		for (itVector iter = m_messages.begin(); iter != m_messages.end(); ++iter) 
		{ 
			buf.append(strFix);
			buf.append(*iter);
			buf.append("\n");
		} 
		buf.erase(buf.size()-2);
	}

	return buf;
}

std::string  LogLayout::NowStringNoYear()
{
 	struct tm* tmTemp;
#ifdef WIN32
    struct _timeb tb;
    _ftime(&tb);
	tmTemp = localtime(&tb.time);
	if (tmTemp == NULL)
		return std::string();
#else		// no windows
    struct timeb tb;
	ftime(&tb);
	struct tm tmTemp2;
	localtime_r(&tb.time, &tmTemp2);
	tmTemp = &tmTemp2;
#endif
	int tm_mon = (*tmTemp).tm_mon;
	int tm_mday = (*tmTemp).tm_mday;
	int tm_hour = (*tmTemp).tm_hour;
	int tm_min = (*tmTemp).tm_min;
	int tm_sec = (*tmTemp).tm_sec;
	int tm_millisec = tb.millitm;
	tm_mon += 1;			//[1,12]
	char szTime[32];
#ifdef WIN32
	_snprintf(szTime,32,"%02d.%02d %02d:%02d:%02d.%03d",
		tm_mon, tm_mday, tm_hour, tm_min, tm_sec, tm_millisec);
#else
	snprintf(szTime,32,"%02d.%02d %02d:%02d:%02d.%03d",
		tm_mon, tm_mday, tm_hour, tm_min, tm_sec, tm_millisec);
#endif

	return szTime;
}

//////////////////////////////////////LogManager////////////////////////////////////
LogManager::LogManager()
{
    m_log_impl = new Logger_impl();
}

LogManager::~LogManager()
{

}

void LogManager::Open(
	const char* pszTag,
	bool bConsoleEnable,
	bool bFileEnable,
	bool bAsync,
	int nBufferSize,
	int nLogVolumeCount,
	int nLogVolumeSize)	
{
	if (NULL != m_log_impl)
		m_log_impl->Open(
			pszTag,
			bConsoleEnable,
			bFileEnable,
			bAsync,
			nBufferSize,
			nLogVolumeCount,
			nLogVolumeSize);
}

void LogManager::Close()
{
    if (m_log_impl)
    {
        m_log_impl->Close();
        
        delete m_log_impl;
        m_log_impl = NULL;
    }
}

int LogManager::GetLogLevel()
{
	if (NULL != m_log_impl)
		return m_log_impl->GetLogLevel();
	else
		return 0;
}

void LogManager::SetLogLevel(int nLevel)
{
	if (NULL != m_log_impl)
		m_log_impl->SetLogLevel(nLevel);
}

void LogManager::SetLogCallback(const LogConfigure& log_config)
{
	if (NULL != m_log_impl)
	{
		m_log_impl->SetLogMode(log_config.log_mode);
		m_log_impl->SetLogLevel(log_config.log_level);
		m_log_impl->SetCallback(log_config);
	}
}

void LogManager::SetLogFileMaxLine(int maxLine)
{
	if (NULL != m_log_impl)
		m_log_impl->SetLogFileMaxLine(maxLine);
}

void LogManager::SetLogFilePath(const char* pszDir)
{
	if (NULL != m_log_impl)
		m_log_impl->SetLogFilePath(pszDir);

	if (0!= pszDir)
	{
		CreateLongDirectory(pszDir);
	}
}

bool LogManager::CreateLongDirectory(const std::string& strDirectory)
{
	std::string::size_type index1 = std::string::npos, index2 = std::string::npos;
	std::string sNew;
	std::string slashset="/\\";
	while(1)
	{
		index2 = index1 + 1;
		index1 = strDirectory.find_first_of(slashset,index2);
		if(index1 == std::string::npos)
			break;
		if(index1 == 0)
			continue;
		if(strDirectory[index1 - 1] == ':')
			continue;
		sNew = strDirectory.substr(0, index1);
#ifdef WIN32
		_mkdir(sNew.c_str());    
#else
        mkdir(sNew.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
	}
	return true;
}

void LogManager::Init(LogConfigure& log_config)
{
    if (strlen(log_config.log_path) == 0)
    {
        strcpy(log_config.log_path, "./");
    }

    std::string strPath = log_config.log_path;

    if(strPath.length() && strPath.at(strPath.length()-1)!='/')
        strPath.append("/");

    if (log_config.log_mode & ONLY_WRITE_LOG)
    {
        CreateLongDirectory(strPath);
        SetLogFilePath(log_config.log_path);
        SetLogLevel(log_config.log_level);
        Open(log_config.prefix,false,true,false,0,log_config.roll_volumes,log_config.volume_size);
    }

    if (log_config.log_mode & APP_WRITE_LOG)
    {
        SetLogCallback(log_config);
    }
}

bool LogManager::push_check_delete_command()
{
	if (NULL != m_log_impl)
	{
		return m_log_impl->push_asyn_check_delete_command();
	}
	else
	{
		return false;
	}
}
    
//////////////////////////////////////LogThread////////////////////////////////////
LogThread::LogThread(Logger_impl* impl):m_log_queue(4 * 1024 * 1024)
{
    m_log_impl = impl;
}

LogThread::~LogThread()
{

}

void LogThread::run(void* lpParam)
{
	LogEvent *pEvent = NULL;
	while(m_bRunning)
	{
		if(!m_log_queue.Pop(pEvent,500))
		{
			msleep(50);
			continue;
		}

		if (pEvent == NULL)
		{
			continue;
		}

		process_event(pEvent);
		if (pEvent)
		{
			delete pEvent;
			pEvent = NULL;
		}
	}
    
    // process render at log queue
    for (size_t i=0; i<m_log_queue.GetSize(); i++) {
        if (m_log_queue.Pop(pEvent, 500)) {
            if (pEvent != NULL) {
                process_event(pEvent);
                delete pEvent;
                pEvent = NULL;
            }
        }
    }
}

int LogThread::process_event(LogEvent *pEvent)
{
	int nRet = 0;
	do 
	{
		if (pEvent == NULL)
		{
			nRet = 1;
			break;
		}

		switch(pEvent->get_event_type())
		{
		case LET_LOG:
			nRet = process_log_event(pEvent);
			break;
		case LET_FLUSH:
			nRet = process_flush_event(pEvent);
			break;
		case LET_CHECK_DELETE:
			nRet = process_check_delete_event(pEvent);
			break;
		default:
			break;
		}
	} while (0);
	return nRet;
}

int LogThread::process_log_event(LogEvent *pEvent)
{
	int nRet = 0;
	do 
	{
		if (strlen(pEvent->get_line()) <= 0)
		{
			nRet = 1;
			break;
		}

		if (NULL != m_log_impl)
			m_log_impl->asyncAppendLine(pEvent->get_line());
	} while (0);
	return nRet;
}

int LogThread::process_flush_event(LogEvent *pEvent)
{
	int nRet = 0;
	do 
	{
		if (NULL != m_log_impl)
			m_log_impl->flush_cache();
	} while (0);
	return nRet;
}

int LogThread::process_check_delete_event(LogEvent *pEvent)
{
	int nRet = 0;
	do 
	{
		if (NULL != m_log_impl)
			m_log_impl->check_file_delete();
	} while (0);
	return nRet;
}


bool LogThread::push_log(const std::string& strMsg)
{
	bool bRet = true;
	do 
	{
		if (!strMsg.empty())
		{
			LogEvent *pEvent = new LogEvent(LET_LOG,strMsg);
			m_log_queue.BlockPush(pEvent);
		}
	} while (0);
	return bRet;
}

bool LogThread::push_flush_command()
{
	bool bRet = true;
	do 
	{
		LogEvent *pEvent = new LogEvent(LET_FLUSH);
		if (!m_log_queue.Push(pEvent))//A flush event is important so it can push the oldest event out of the queue.
			delete pEvent;
	} while (0);
	return bRet;
}

bool LogThread::push_check_delete_command()
{
	bool bRet = true;
	do 
	{
		LogEvent *pEvent = new LogEvent(LET_CHECK_DELETE);
		if (!m_log_queue.Push(pEvent))//A flush event is important so it can push the oldest event out of the queue.
			delete pEvent;

	} while (0);
	return bRet;
}

} //namespace VOIP
