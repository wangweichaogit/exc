// Logger_impl.cpp: implementation of the Logger_impl class.
//
//////////////////////////////////////////////////////////////////////

#include "comLib/Logger_impl.h"
#include "comLib/SysUtil.h"

#include <algorithm>
#include <sys/timeb.h>
#include <iostream>
#include <fstream>

#ifdef WIN32
#   include <time.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace DHT
{
    
///////////////////////////////////LogAppender///////////////////////////////////
LogAppender::LogAppender()
{
    m_bAsyncMode = false;
}

LogAppender::~LogAppender()
{

}

///////////////////////////////////LogAppenderConsole///////////////////////////////////////
LogAppenderConsole::LogAppenderConsole()
{

}

LogAppenderConsole::~LogAppenderConsole()
{

}

void LogAppenderConsole::AppendLine(const char* msg)
{
	if (is_async_mode())
	{
		printf("%s",msg);
	}
	else
	{
		printf("%s\n",msg);
	}
}

void LogAppenderConsole::Open(const char *pszTag)
{

}

void LogAppenderConsole::Close()
{

}

////////////////////////////////////LogAppenderFile//////////////////////////////////
LogAppenderFile::LogAppenderFile()
{
#ifdef WIN32
    m_cszLogFileFullPathTag=".\\";
#else
    m_cszLogFileFullPathTag="./";
#endif
    
    m_MaxFileLine   = 100*1024;
    m_strDir        = "";
    m_nVolumeSize   = 10 * 1024 * 1024;    
}

LogAppenderFile::~LogAppenderFile()
{
	Close();
}

void LogAppenderFile::AppendLine(const char* msg)
{
	//Async mode do io in a same thread,so lock is unnecessary
	if (is_async_mode())
	{
		doAppendLine(msg);
	}
	else
	{
		SingleMutex s(m_lock);
		doAppendLine(msg);
	}
}

void LogAppenderFile::doAppendLine(const char* msg)
{
	try
	{
		if (m_nRollVolumes)
		{
			if (m_nVolumeSize && (m_nSize >= m_nVolumeSize))
			{
				NewLog();
				m_nSize = 0;
			}
			m_nSize += strlen(msg);
		}
		else
		{
			if(m_nTotalLogTimes >= m_MaxFileLine )
			{
				NewLog();
				m_nTotalLogTimes = 0;
			}
			m_nTotalLogTimes++;
		}

		if (m_cLogFile.is_open() == false) return;

		if (is_async_mode())
		{
			m_cLogFile << msg << std::flush;// << std::flush;
		}
		else
		{
			m_cLogFile << msg << std::endl;// << std::flush;
		}
	}catch(...){}
}

void LogAppenderFile::Open(const char *pszTag)
{
	if (is_async_mode())
	{
		doOpen(pszTag);
	}
	else
	{
		SingleMutex s(m_lock);
		doOpen(pszTag);
	}
}

void LogAppenderFile::doOpen(const char *pszTag)
{
	static bool bInit = false;
	if ( true == bInit )
		return ;

	std::string strTag;
	if ( pszTag != NULL)
	{
		strTag = pszTag;
	}
	else
	{
		strTag = "log";
	}

	m_cszLogFileFullPathTag.append(strTag);

	if (m_nRollVolumes)
	{
		//如果处于回滚模式 得到原始文件名
		m_strFileName = GetFileName(strTag);

		// Get volume index
		//得到目录下的最大日志索引(最旧的日志文件)
		m_nIndex = getMaxIndex(m_strDir, m_strFileName); 

		if (m_nIndex > m_nRollVolumes)
		{
			//如果最大索引大于最大的日志数说明最大日志数已经被改变，这样多余的日志文件将不再处理范围之内
			m_nIndex = m_nRollVolumes;
		}

#ifdef WIN32
		char slash = '\\';
#else
		char slash = '/';
#endif

		//得到当前日志文件的绝对路径
		std::string pathname = SysUtil::util_path_cat(m_strDir, m_strFileName, slash);
		//得到当前日志文件的大小
		m_nSize = SysUtil::util_get_file_size(pathname);
		//以append方式打开文件
		Open(true);
	}
	else
	{
		Open(true);
	}
}

void LogAppenderFile::Close()
{
	if (is_async_mode())
	{
		doClose();
	}
	else
	{
		SingleMutex s(m_lock);
		doClose();
	}
}

void LogAppenderFile::doClose()
{
    if (m_cLogFile.is_open()) 
	{
        m_cLogFile.clear();		//open() never clears any state flags
        m_cLogFile.close();
    } else {
        printf("log file had already closed!!!");
    }
}

//必须是相对路径，不支持绝对路径
void LogAppenderFile::SetLogFilePath(const char* pszDir)
{
	if (0 != pszDir)
	{
		//////////////////////////////////////日志回滚路径////////////////////////////////////
		m_strDir = pszDir;

#ifdef WIN32
		char slash = '\\';
#else
		char slash = '/';
#endif

		//去掉最后的/
		char lastc = m_strDir.at(m_strDir.length() - 1);
		if (lastc == slash || lastc == '/')
		{
			m_strDir = m_strDir.substr(0,m_strDir.length() - 1);
		}

		//处理上一级目录
		std::string strCwd = SysUtil::util_get_cwd();
		while(1)
		{
			std::string::size_type dpos1 = m_strDir.find("../");
			std::string::size_type dpos2 = m_strDir.find("..\\");
			if (dpos1 == 0 || dpos2 == 0) //如果有上一级目录
			{
				std::string::size_type lspos1 = strCwd.find_last_of(slash);
				std::string::size_type lspos2 = strCwd.find_last_of('/');
				if (lspos1 != strCwd.npos || lspos2 != strCwd.npos)
				{
					std::string::size_type lspos;
					if (lspos1 != strCwd.npos && lspos2 != strCwd.npos)
					{
						lspos = std::max<std::string::size_type>(lspos1,lspos2);
					}
					else if (lspos1 != strCwd.npos)
					{
						lspos = lspos1;
					}
					else
					{
						lspos = lspos2;
					}
					strCwd = strCwd.substr(0,lspos);
					m_strDir = m_strDir.substr(3,m_strDir.length() - 3);
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}

		//获取绝对路径
#ifdef WIN32
		if ((m_strDir.size() > 2) && ( *(m_strDir.begin() + 1) != ':'))
			m_strDir = strCwd + slash + m_strDir;
#else
		if (*m_strDir.begin() != slash)
			m_strDir = strCwd + slash + m_strDir;
#endif
		//////////////////////////////////////////////////////////////////////////

		m_cszLogFileFullPathTag = pszDir;
		if (m_cszLogFileFullPathTag.at(m_cszLogFileFullPathTag.length() - 1) != '\\' && 
			m_cszLogFileFullPathTag.at(m_cszLogFileFullPathTag.length() - 1) != '/') //Compatible with Windows(both supported) and Linux
		{
			m_cszLogFileFullPathTag += slash;
		}
	}
}

void LogAppenderFile::SetLogFileMaxLine(int MaxLine)
{
	if (MaxLine<100) return;
	m_MaxFileLine = MaxLine;
}

void LogAppenderFile::Open(bool bAppend)
{
#ifdef WIN32
	std::ios_base::open_mode mode = bAppend?(std::ios::out|std::ios::app):(std::ios::out|std::ios::trunc);
#else
#ifdef __IOS__
	std::ios_base::openmode mode = bAppend?(std::ios::out|std::ios::app):(std::ios::out|std::ios::trunc);
#else
	std::_Ios_Openmode mode = bAppend?(std::ios::out|std::ios::app):(std::ios::out|std::ios::trunc);
#endif
#endif

	std::locale::global(std::locale("")); //支持中文路径
	if (m_nRollVolumes > 0)
	{
#ifdef WIN32
		char slash = '\\';
#else
		char slash = '/';
#endif
		std::string strFullPath = m_strDir + slash + m_strFileName;
		m_cLogFile.open(strFullPath.c_str(), mode);
	}
	else
	{
		m_cszLogFileFullPath = GetFileName();
		m_cLogFile.open(m_cszLogFileFullPath.c_str(), mode);
	}
	std::locale::global(std::locale("C"));
}

std::string LogAppenderFile::GetFileName()
{
 	struct tm* tmTemp;
#ifdef WIN32
    struct _timeb tb;
    _ftime(&tb);
	tmTemp = localtime(&tb.time);
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
	tm_mon += 1;			//[1,12]
	char szTime[32];
#ifdef WIN32
	_snprintf(szTime,32,"%02d.%02d.%02d-%02d-%02d",
		tm_mon, tm_mday, tm_hour, tm_min, tm_sec);
#else
	snprintf(szTime,32,"%02d.%02d.%02d-%02d-%02d",
		tm_mon, tm_mday, tm_hour, tm_min, tm_sec);
#endif

	std::string cszFileName ;
	cszFileName = m_cszLogFileFullPathTag;
#ifndef __IOS__ 
	// IOS APP filename no timestamp subfix:xxxxkkkkzzzz.log
	cszFileName += ".";
	cszFileName += szTime;
	cszFileName +=  ".log"; 
#endif
	
	return cszFileName;
}

std::string LogAppenderFile::GetFileName(const std::string &strTag)
{
	std::string cszFileName ;
	cszFileName = strTag;
	cszFileName +=  ".log";

	return cszFileName;
}

void LogAppenderFile::NewLog()
{
    //LOCK(&m_FileLock);
	if (m_nRollVolumes > 0)
	{
		RollingVolumes();
	}
	else
	{
		Close();
		Open(true);
	}
	//UNLOCK(&m_FileLock);
}

void LogAppenderFile::RollingVolumes()
{
	// Close the streamer first
	Close();

	if (m_nRollVolumes > 1)
	{
#ifdef WIN32
		char slash = '\\';
#else
		char slash = '/';
#endif

		// 1. Delete last volume(_index) if _index > _maxIndex
		std::string strOldestLog = m_strFileName;

		// + '.nn'
		if ( (m_nIndex > 0) && (m_nIndex >= m_nRollVolumes - 1)) 
		{
			m_nIndex = m_nRollVolumes - 1;

			strOldestLog += ".";
			strOldestLog += SysUtil::util_itoa(m_nIndex);

			SysUtil::util_delete_file(SysUtil::util_path_cat(m_strDir, strOldestLog, slash));
		}//if ( (m_nIndex > 0) && (m_nIndex >= m_nMaxIndex - 1)) 

		// 2. Rename [1, _index - 1] volumes to which+1
		std::vector<std::string> v;
		SysUtil::util_list_dir(m_strDir, v);

		// sort the list reversely
		std::sort(v.rbegin(), v.rend(), LogFilenameCompare);

		// Rename
		for (std::vector<std::string>::const_iterator i = v.begin();i != v.end(); ++i) 
		{
			int k = getIndex(m_strFileName, *i);
			if (k < (m_nRollVolumes - 1) && k >= 0) 
			{
				// do renaming
				std::string newLog(m_strFileName);
				newLog += ".";
				newLog += SysUtil::util_itoa(k + 1);

				std::string strSrc = SysUtil::util_path_cat(m_strDir, *i, slash);
				std::string strDst = SysUtil::util_path_cat(m_strDir, newLog, slash);
				if (SysUtil::util_rename(strSrc,strDst) != UTIL_SUCCESS) 
				{
					// do nothing?
				}//if (util_rename(util_path_cat(_dir, *i, slash),
			}//if (k < (m_nMaxIndex - 1) && k >= 0) 
		}//for (std::vector<std::string>::const_iterator i = v.begin();i != v.end(); ++i) 
	}//if (m_nMaxIndex > 1)

	Open(true);
	m_nIndex++;
}

int LogAppenderFile::getIndex(const std::string& p, const std::string& n)
{
	int i = 0;

	// Name matching
	if (p.compare(n.substr(0, p.size())) == 0) {

		std::string idx = n.substr(p.size(), 
			n.find(".", p.size(), n.size() - p.size()));

		// idx should be '.nn' or '.nn.gz'
		if (!idx.empty()) {
			i = atoi(idx.substr(1).c_str());
		}
	} else {
		i = -1;
	}

	return i;
}

bool LogAppenderFile::LogFilenameCompare(const std::string& s1, const std::string& s2)
{
	int index1 = 0;
	int index2 = 0;

	if (s1.empty() || s2.empty()) {
		return false;
	}

	std::vector<std::string> v = SysUtil::util_strsplit(s1, '.');
	if (v.size() <= 0)
		index1 = 0;
	else {
		if (v[v.size() - 1] == "gz")
			index1 = atoi(SysUtil::util_strtrim(v[v.size() - 2]).c_str());
		else
			index1 = atoi(SysUtil::util_strtrim(v[v.size() - 1]).c_str());
	}

	v = SysUtil::util_strsplit(s2, '.');
	if (v.size() <= 0)
		index2 = 0;
	else {
		if (v[v.size() - 1] == "gz")
			index2 = atoi(SysUtil::util_strtrim(v[v.size() - 2]).c_str());
		else
			index2 = atoi(SysUtil::util_strtrim(v[v.size() - 1]).c_str());
	}

	if (index1 >= index2)
		return false;
	else
		return true;
}

void LogAppenderFile::set_roll_volumes(int nVolumes)
{
	m_nRollVolumes = (nVolumes <= 0)?0:nVolumes;
}

int LogAppenderFile::getMaxIndex(const std::string& dir, const std::string& filename)
{
	// Log file name should be 'test.log.nn' 
	// or compressed form 'test.log.nn.gz'

	int c = 0;
	std::vector<std::string> v;

	// First, get the list of directory
	if (SysUtil::util_list_dir(dir, v) == UTIL_SUCCESS) {
		for (std::vector<std::string>::const_iterator i = v.begin();
			i != v.end(); ++i) 
		{
			int k = getIndex(filename, *i);
			if (c < k)
				c = k;
		}
	}

	return c;
}

void LogAppenderFile::check_file_delete()
{
#ifdef WIN32
	char slash = '\\';
#else
	char slash = '/';
#endif
	std::string strFullPath;
	if (m_nRollVolumes > 0)
	{
		strFullPath = m_strDir + slash + m_strFileName;
	}
	else
	{
		std::string strFullPathName = m_cszLogFileFullPath;
		std::string::size_type spos1 = strFullPathName.find_last_of(slash);
		std::string::size_type spos2 = strFullPathName.find_last_of('/');
		if (spos1 != std::string::npos || spos2 != std::string::npos)
		{
			std::string::size_type spos;
			if (spos1 != std::string::npos && spos2 != std::string::npos)
			{
				spos = std::max<std::string::size_type>(spos1,spos2);
			}
			else if (spos1 != std::string::npos)
			{
				spos = spos1;
			}
			else
			{
				spos = spos2;
			}
			strFullPathName = strFullPathName.substr(spos + 1,strFullPathName.length() - spos - 1);
		}
		strFullPath = m_strDir + slash + strFullPathName;
	}

	if (!SysUtil::util_is_file_exist(strFullPath))
	{
		m_nSize = 0;
		Close();
		Open(true);
	}
}

///////////////////////////////LogAppenderShareMem///////////////////////////////////////
LogAppenderShareMem::LogAppenderShareMem()
{
}

LogAppenderShareMem::~LogAppenderShareMem()
{

}

void LogAppenderShareMem::Open(const char *pszTag)
{

}

void LogAppenderShareMem::AppendLine(const char* msg)
{

}
    
///////////////////////////////////////Logger_impl///////////////////////////////////
Logger_impl::Logger_impl()
{
    m_nLogLevel = 11;
    m_eLogMode = ONLY_WRITE_LOG;
    
    pfnDhnLogCallback   = NULL;
    m_pLogContext       = NULL;
    m_szBuffer          = NULL;
    m_nBufferSize       = 0;
    m_bAsyncMode        = false;
    m_nBufferPos        = 0;
    m_bEnableConsole    = true;
    m_bEnableFile       = true;
    m_bAppWriteLog      = false;

	m_log_thread = NULL;
}

Logger_impl::~Logger_impl()
{
	if (NULL != m_log_thread)
	{
		delete m_log_thread;
	}
}

void Logger_impl::Open(
                       const char* pszTag,
                       bool bConsoleEnable,
                       bool bFileEnable,
                       bool bAsync,
                       int nBufferSize,
                       int nLogVolumeCount,
                       int nLogVolumeSize)
{
    m_log_thread = NULL;
    
    SetRollVolumes(nLogVolumeCount);
    SetLogVolumeSize(nLogVolumeSize);
    m_bAsyncMode = bAsync;
    
    m_bEnableConsole = bConsoleEnable;
    if (m_bEnableConsole)
    {
        m_appenders.push_back(&lac);
        lac.Open(pszTag);
        lac.set_async_mode(bAsync);
    }
    
    m_bEnableFile = bFileEnable;
    if (m_bEnableFile)
    {
        m_appenders.push_back(&laf);
        laf.Open(pszTag);
        laf.set_async_mode(bAsync);
    }
    
    if (m_bAsyncMode)
    {
        init_buffer(nBufferSize);
        m_log_thread = new LogThread(this);
        m_log_thread->SynStart(NULL,5000);
    }
}

void Logger_impl::Close()
{
    if (m_bAsyncMode)
    {
        m_log_thread->SynStop(5000);
        if (m_log_thread) {
            delete m_log_thread;
            m_log_thread = NULL;
        }
        
        flush_cache();
        destroy_buffer();
    }
    m_appenders.clear();
    
    if (m_bEnableConsole)
    {
        lac.Close();
    }
    
    if (m_bEnableFile)
    {
        laf.Close();
    }
    
    m_bAsyncMode = 0;
    m_nBufferPos = 0;
    m_bAsyncMode = false;
}

int Logger_impl::GetLogLevel()
{
    return m_nLogLevel;
}
void Logger_impl::SetLogLevel(int nLevel)
{
    m_nLogLevel = nLevel;
}

void Logger_impl::SetCallback(const LogConfigure& log_config)
{
    m_bAppWriteLog = (log_config.log_mode == APP_WRITE_LOG);
    pfnDhnLogCallback = log_config.pfnLogCallback;
    m_pLogContext = log_config.log_context;
}

void Logger_impl::AppendLine(const std::string& msg)
{
    if (m_eLogMode & APP_WRITE_LOG)
    {
        if (pfnDhnLogCallback != NULL)
        {
            pfnDhnLogCallback(m_pLogContext,m_nLogLevel,msg.c_str());
        }
    }
    else
    {
        if (m_bAsyncMode)
        {
            m_log_thread->push_log(msg + "\n");
        }
        else
        {
            syncAppendLine(msg);
        }
    }
}

int Logger_impl::init_buffer(int nSize)
{
    int nRet = 0;
    do
    {
        if (nSize <= 0)
        {
            nRet = 1;
            break;
        }
        
        if (m_szBuffer != NULL)
        {
            delete [] m_szBuffer;
            m_szBuffer = NULL;
        }
        
        m_szBuffer = new char[nSize];
        memset(m_szBuffer,0,nSize);
        m_nBufferSize = nSize;
    } while (0);
    return nRet;
}

int Logger_impl::destroy_buffer()
{
    int nRet = 0;
    do
    {
        if (m_szBuffer != NULL)
        {
            delete [] m_szBuffer;
            m_szBuffer = NULL;
        }
        m_nBufferSize = 0;
    } while (0);
    return nRet;
}

void Logger_impl::flush(char *szBuffer)
{
    if (szBuffer == NULL)
    {
        return;
    }
    
    typedef std::vector<LogAppender*>::iterator itLog;
    for (itLog iter = m_appenders.begin(); iter != m_appenders.end(); ++iter)
    {
        LogAppender* pla = *iter;
        pla->AppendLine(szBuffer);
    }
}

void Logger_impl::reset_buffer()
{
    memset(m_szBuffer,0,m_nBufferSize);
    m_nBufferPos = 0;
}

void Logger_impl::syncAppendLine(const std::string& msg)
{
    flush((char*)msg.c_str());
}

void Logger_impl::asyncAppendLine(const std::string& msg)
{
    //If no buffer exists,directly flush into file
    if (m_szBuffer == NULL || m_nBufferSize == 0)
    {
        flush((char*)msg.c_str());
        return;
    }
    
    int len = (int)msg.size();
    int i = 0;
    //If current log line add current cached log length exceed buffer size
    if (len + m_nBufferPos >= m_nBufferSize)
    {
        //If some log cached in buffer,flush it
        if (m_nBufferPos > 0)
        {
            flush(m_szBuffer);
            reset_buffer();
        }
        
        //If current log line length larger than buffer size,flush it,
        //flush unit is buffer size
        while (len >= m_nBufferSize)
        {
            strncpy(m_szBuffer, msg.c_str() + i * (m_nBufferSize - 1), m_nBufferSize - 1);
            flush(m_szBuffer);
            reset_buffer();
            i++;
            len -= (m_nBufferSize - 1);
        }
        
        if (len > 0)
        {
            // append to buffer
            strncpy(m_szBuffer, msg.c_str() + i * (m_nBufferSize - 1), len);
            m_nBufferPos += len;
        }
    } 
    else 
    {
        // append to buffer
        strncpy(m_szBuffer + m_nBufferPos, msg.c_str(), len);
        m_nBufferPos += len;
    }
    
}

void Logger_impl::flush_cache()
{
    if (m_szBuffer && m_nBufferPos > 0) 
    {
        flush(m_szBuffer);
        reset_buffer();
    }
}

void Logger_impl::check_file_delete()
{
    laf.check_file_delete();
}

void Logger_impl::SetRollVolumes(int nRollVolumes)
{
    laf.set_roll_volumes(nRollVolumes);
}

void Logger_impl::SetLogVolumeSize(int nVolumeSize)
{
    laf.set_volume_size(nVolumeSize);
}
    
void Logger_impl::SetLogFilePath(const char* pszDir)
{
    laf.SetLogFilePath(pszDir);
}
    
void Logger_impl::SetLogFileMaxLine(int MaxLine)
{
    laf.SetLogFileMaxLine(MaxLine);
}

bool Logger_impl::push_asyn_check_delete_command()
{
	if (NULL != m_log_thread)
	{
		return m_log_thread->push_check_delete_command();
	}
	else
	{
		check_file_delete();//sync write log. direct check file delete.
		return true;
	}
}

} //namespace VOIP

