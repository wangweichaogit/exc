// Logger_impl.h: interface for the Logger_impl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGGER_IMPL_H__204F64E0_D755_4A5A_8AC1_19204D5AC0AB__INCLUDED_)
#define AFX_LOGGER_IMPL_H__204F64E0_D755_4A5A_8AC1_19204D5AC0AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "comLib/Logger.h"
#include "comLib/Mutex.h"
#include "comLib/Logger_type.h"

namespace DHT
{

////////////////////////////////////LogAppender//////////////////////////////////////
class COMLIBAPI LogAppender
{
public:
	LogAppender();
	virtual ~LogAppender();

	virtual void Open(const char* pszTag)=0;
	virtual void Close() = 0;
	virtual void AppendLine(const char* msg)=0;
    
    bool is_async_mode(){return m_bAsyncMode;}
    void set_async_mode(bool bflag) { m_bAsyncMode = bflag; }
public:
    bool m_bAsyncMode;
};

////////////////////////////////////LogAppenderConsole//////////////////////////////////////
class LogAppenderConsole : public LogAppender  
{
public:
	LogAppenderConsole();
	virtual ~LogAppenderConsole();

public:
	void AppendLine(const char* msg);
	void Open(const char* pszTag);
	void Close();
};

//////////////////////////////////////LogAppenderFile////////////////////////////////////
class COMLIBAPI LogAppenderFile : public LogAppender  
{
public:
	LogAppenderFile();
	virtual ~LogAppenderFile();

public:
	void AppendLine(const char* msg);
	void doAppendLine(const char* msg);
	void Open(const char* pszTag);
	void doOpen(const char* pszTag);
	void Close();
	void doClose();
	void SetLogFilePath(const char* pszDir);
	void SetLogFileMaxLine(int MaxLine);

	//For rolling volumes
	void RollingVolumes();
	int getIndex(const std::string& p, const std::string& n);
	
    static bool LogFilenameCompare(const std::string& s1, const std::string& s2);
    
	void set_roll_volumes(int nVolumes);
	int getMaxIndex(const std::string& dir, const std::string& filename);
	void set_volume_size(long lVolumeSize){if (lVolumeSize) m_nVolumeSize = lVolumeSize*1024;}
	void check_file_delete();

private:
	void Open(bool bAppend = false);
	void NewLog();
	std::string GetFileName();
	std::string GetFileName(const std::string &strTag);

private:
	Mutex m_lock;
	std::ofstream m_cLogFile;
	std::string	m_cszLogFileFullPath;
	int m_nTotalLogTimes;
    
    std::string m_cszLogFileFullPathTag;
	int m_MaxFileLine;

	//For rolling volumes
	int m_nIndex;
	std::string m_strDir;
	std::string m_strFileName;
	int m_nRollVolumes;
	int m_nSize;
	long m_nVolumeSize;
};

class LogAppenderShareMem : public LogAppender  
{
public:
	void AppendLine(const char* msg);
	void Open(const char* pszTag);
	void Close(){}
	LogAppenderShareMem();
	virtual ~LogAppenderShareMem();
};

////////////////////////////////////Logger_impl//////////////////////////////////////
class COMLIBAPI Logger_impl
{
public:
    Logger_impl();
    ~Logger_impl();
    
    void Open(
              const char* pszTag=0,
              bool bConsoleEnable = true,
              bool bFileEnable = true,
              bool bAsync = false,
              int nBufferSize = 0,
              int nLogVolumeCount = 0,
              int nLogVolumeSize = 0);
    void Close();
    int GetLogLevel();						//取得Tag的Level.
    void SetLogLevel(int nLevel);			//设置Tag的Level.
    void SetCallback(const LogConfigure& log_config); //设置回调
    void AppendLine(const std::string& msg);
    void SetLogMode(LogMode eMode){m_eLogMode = eMode;}
    int init_buffer(int nSize);
    int destroy_buffer();
    void set_log_mode(bool bAsync){m_bAsyncMode = bAsync;}
    void flush(char *szBuffer);
    void reset_buffer();
    void syncAppendLine(const std::string& msg);
    void asyncAppendLine(const std::string& msg);
    void flush_cache();
    void check_file_delete();
    bool is_async_mode(){return m_bAsyncMode;}
    void SetRollVolumes(int nRollVolumes);
    void SetLogVolumeSize(int nVolumeSize);
    bool IsAppWriteLog() { return m_bAppWriteLog; }
    void SetLogFilePath(const char* pszDir);
    void SetLogFileMaxLine(int MaxLine);

	bool push_asyn_check_delete_command();//切线程
    
private:
    Mutex m_lock;
    int m_nLogLevel;
    std::vector<LogAppender*> m_appenders;
    bool m_bAppWriteLog;
    LogMode m_eLogMode;
    PrintLogEntryCallback pfnDhnLogCallback;
    
    void *m_pLogContext;
    char *m_szBuffer;
    int m_nBufferSize;
    bool m_bAsyncMode;
    int m_nBufferPos;
    bool m_bEnableConsole;
    bool m_bEnableFile;
    
    LogAppenderConsole		lac;
    LogAppenderFile			laf;
    LogAppenderShareMem		lasm;
    
    LogThread* m_log_thread;
};
    
} //namespace VOIP

#endif // !defined(AFX_LOGGER_IMPL_H__204F64E0_D755_4A5A_8AC1_19204D5AC0AB__INCLUDED_)
