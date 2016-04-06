#include "log.h"

static FILE* __fileout__ = NULL;
static boost::mutex* mutexDebugLog = NULL;
static bool fReopenDebugLog = true;
static bool fPrintToConsole = true;
static bool fPrintToDebugLog = true;
static boost::once_flag debugPrintInitFlag = BOOST_ONCE_INIT;

static int FileWriteStr(const std::string &str, FILE *fp)
{
    return fwrite(str.data(), 1, str.size(), fp);
}

static void DebugPrintInit()
{
    mutexDebugLog = new boost::mutex();
}

boost::filesystem::path GetDefaultDataDir()
{
    namespace fs = boost::filesystem;

    fs::path pathRet;
    char* pszHome = getenv("PWD");
    pathRet = fs::path(pszHome);
    return pathRet;
}

int64_t GetTimeMicros()
{
    int64_t now = (boost::posix_time::microsec_clock::universal_time() -
                   boost::posix_time::ptime(boost::gregorian::date(1970,1,1))).total_microseconds();
    return now;
}
std::string DateTimeStrFormat(const char* pszFormat, int64_t nTime)
{
    // std::locale takes ownership of the pointer
    std::locale loc(std::locale::classic(), new boost::posix_time::time_facet(pszFormat));
    std::stringstream ss;
    ss.imbue(loc);
    ss << boost::posix_time::from_time_t(nTime);
    return ss.str();
}

static std::string GetCurTime()
{
	std::string timeStr;
	timeStr =  DateTimeStrFormat("%Y-%m-%d %H:%M:%S", GetTimeMicros()/1000000);
	return timeStr;
}


void OpenDebugLog()
{
	#if 0
    boost::call_once(&DebugPrintInit, debugPrintInitFlag);
    boost::mutex::scoped_lock scoped_lock(*mutexDebugLog);
	#endif
    boost::filesystem::path pathDebug = GetDefaultDataDir() / "debug.log";
    __fileout__ = fopen(pathDebug.string().c_str(), "a");
    if (__fileout__) setbuf(__fileout__, NULL); // unbuffered
    fReopenDebugLog = false;
}

int LogInfoStr(const std::string &str)
{
		int ret = 0;
    std::string LogStr;
    std::string timeStr = GetCurTime();
		LogStr = '[' + timeStr + ']' + str;
		
		if (fPrintToConsole)
    {
        ret = fwrite(LogStr.data(), 1, LogStr.size(), stdout);
        fflush(stdout);
    }
   	if (fPrintToDebugLog)
    	{

    		boost::call_once(&DebugPrintInit, debugPrintInitFlag);
        boost::mutex::scoped_lock scoped_lock(*mutexDebugLog);
				if (fReopenDebugLog) {
					OpenDebugLog();
				}
        ret = FileWriteStr(LogStr, __fileout__);
    	}
    return ret;
}
