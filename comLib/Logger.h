// Logger.h: interface for the Logger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGGER_H__0C0D5FA0_DAC4_4BB8_B2A7_EFD14C996639__INCLUDED_)
#define AFX_LOGGER_H__0C0D5FA0_DAC4_4BB8_B2A7_EFD14C996639__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include <fstream>

#include "comLib/comdefine.h"
#include "comLib/Mutex.h"
#include "comLib/Thread.h"
#include "comLib/SafeQueue.h"
#include "comLib/Logger_type.h"
#include "comLib/Stdafx.h"

namespace DHT
{

typedef enum LOG_EVENT_TYPE
{
	LET_LOG = 0,
	LET_FLUSH,
	LET_CHECK_DELETE
}LOG_EVENT_TYPE;

class Logger;
class Logger_impl;

/////////////////////////////////////////LogManager/////////////////////////////////
class COMLIBAPI LogManager  
{
public:
	//log example //2008-08-08 08:08:08.888-DG-[threadid][Tag][str1][str2][str3]this is log-test-message.
	///////////////////////////////////////////////////////////
    LogManager();
    ~LogManager();
    void Open(
		const char* pszTag=0,
		bool bConsoleEnable = true,
		bool bFileEnable = true,
		bool bAsync = false,
		int nBufferSize = 0,
		int nLogVolumeCount = 0,
		int nLogVolumeSize = 0);	//初始化. filename prefix, default is 'log'
	void Close();
	int GetLogLevel();						//取得Tag的Level.
	void SetLogLevel(int nLevel);			//设置Tag的Level.
	void SetLogFilePath(const char* pszDir);
	void SetLogFileMaxLine(int maxLine);
	bool CreateLongDirectory(const std::string& strDirectory);
    void SetLogCallback(const LogConfigure& log_config);
    void Init(LogConfigure& log_cfg);

	bool push_check_delete_command();
private:
    Logger_impl* m_log_impl;
    friend class Logger;
};

class LogLayout
{
public:
	LogLayout();
	LogLayout(char* str1);
	LogLayout(char* str1, char* str2);
	LogLayout(char* str1, char* str2, char* str3);
	void AppendLine(const char* format, ...);
    
private:
    std::string NowStringNoYear();
    std::string GetLevelString(int nLevel2Str);
    std::string GetLayoutString(int nLevel2Str, std::string strClassTag);//time-LV-[threadid][Tag][str1][str2][str3]
    std::string m_strBuf;//[str1][str2][str3]
	//std::string m_strMessage;	//message from format
	std::vector<std::string>       m_messages;
	friend class Logger;
};

// log interface
class COMLIBAPI Logger  
{
public:
	enum	LOG_LEVEL
	{
		LL_FATAL	= 0,	//fatal	error
		LL_ERROR	=1,     //error
		LL_WARN		=3,		//warning
		LL_TRACE	=5,		//trace
		LL_INFO		=7,		//info
		LL_DEBUG	=9,		//debug
		LL_ALL = 99
	};
public:
	Logger(LogManager* log_manager);
    Logger(const char* pszTag, LogManager* log_manager);
    void Init(const char* tag, LogManager* log_manager);    
	virtual ~Logger();
	void Debug(const char* format, ...);    //9
	void Info(const char* format, ...);    //7
	void Trace(const char* format, ...);    //5
	void Warn(const char* format, ...);    //3
	void Error(const char* format, ...);  //  1
	void Fatal(const char* format, ...);  //Level 0
	void Write(const char* format, ...);  //Level -1 , noLevel
	void Append(const char* msg);			//Level -1 , noLevel
    void Append(int level, const char* msg);
	bool isDebugEnabled();
	bool isInfoEnabled();
	bool isTraceEnabled();
	bool isWarnEnabled();
	bool isErrorEnabled();
	bool isFatalEnabled();
	void Debug(LogLayout& lot );    //9
	void Info(LogLayout& lot);    //7
	void Trace(LogLayout& lot);    //5
	void Warn(LogLayout& lot);    //3
	void Error(LogLayout& lot);  //  1
	void Fatal(LogLayout& lot);  //Level 0
	void Write(LogLayout& lot);  //Level -1 , noLevel
	//void SetTag(const char* pszTag);
	void SetTag(const char* format, ...);
private:
	char m_strTag[32];
    Logger_impl* m_log_impl;
};

/////////////////////////////////////LogEvent/////////////////////////////////////
class LogEvent
{
public:
	LogEvent(
		LOG_EVENT_TYPE eType,
        const std::string& strLine = "");
	~LogEvent();

public:
	LOG_EVENT_TYPE get_event_type(){return m_eType;}
	const char* get_line() const {return m_strLine.c_str();}

private:
	LOG_EVENT_TYPE m_eType;
    std::string m_strLine;
};

////////////////////////////////////LogThread//////////////////////////////////////
class LogThread : public Thread
{
public:
    LogThread(Logger_impl* impl);
	virtual ~LogThread();
	bool push_log(const std::string &strMsg);
	bool push_flush_command();
	bool push_check_delete_command();

protected:
	virtual void run(void* lpParam);
	int process_event(LogEvent *pEvent);
	int process_log_event(LogEvent *pEvent);
	int process_flush_event(LogEvent *pEvent);
	int process_check_delete_event(LogEvent *pEvent);

private:
	LogThread();

private:
	SafeQueue<LogEvent*> m_log_queue;
    Logger_impl* m_log_impl;
};

} //namespace VOIP

#endif // !defined(AFX_LOGGER_H__0C0D5FA0_DAC4_4BB8_B2A7_EFD14C996639__INCLUDED_)
