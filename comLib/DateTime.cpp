// DateTime.cpp: implementation of the DateTime class.
//
//////////////////////////////////////////////////////////////////////

#include "comLib/DateTime.h"
#include <stdio.h>
#include <sys/timeb.h>

#ifdef WIN32
#   include <time.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace DHT
{

DateTime::DateTime():m_sec(0)
{
	tmInit();
}

DateTime::DateTime(long time_t)
{
	tmInit();
	m_sec = time_t;
}

DateTime::DateTime(long time_t, int millitm)
{
	tmInit();
	m_sec = time_t;
	tm_millisec = millitm;
}
void DateTime::tmInit()
{
    tm_sec= 0;
    tm_min=0;
    tm_hour= 0;
    tm_mday= 0;
    tm_mon= 0;
    tm_year= 0;
    tm_wday= 0;
    tm_yday= 0;
	tm_millisec =0;
}

void DateTime::GetStructTime()
{
 	struct tm* tmTemp;
#ifdef WIN32
	tmTemp = localtime(&m_sec);
	if (NULL == tmTemp)
		return;
#else		// no windows
	struct tm tmTemp2;
	localtime_r(&m_sec, &tmTemp2);
	tmTemp = &tmTemp2;
#endif
	tm_year = (*tmTemp).tm_year;
	tm_mon = (*tmTemp).tm_mon;
	tm_mday = (*tmTemp).tm_mday;
	tm_hour = (*tmTemp).tm_hour;
	tm_min = (*tmTemp).tm_min;
	tm_sec = (*tmTemp).tm_sec;
	tm_wday = (*tmTemp).tm_wday;
	tm_yday = (*tmTemp).tm_yday;

	tm_year += 1900;   //[1970,2038]
	tm_mon += 1;			//[1,12]
}

DateTime::~DateTime()
{

}

std::string DateTime::ToString()
{
	char szTime[128];
	sprintf(szTime,"%04d.%02d.%02d %02d:%02d:%02d.%03d",
		tm_year, tm_mon, tm_mday, tm_hour, tm_min, tm_sec, tm_millisec);

	return szTime;
}
std::string DateTime::ToStringForCdr()
{
	char szTime[128];
	sprintf(szTime,"%04d%02d%02d%02d%02d%02d",
		tm_year, tm_mon, tm_mday, tm_hour, tm_min, tm_sec);

	return szTime;
}

std::string DateTime::ToStringForUpdate()
{
	char szTime[128];
	sprintf(szTime,"%04d%02d%02d",tm_year, tm_mon, tm_mday);

	return szTime;
}

std::string DateTime::ToStringNoYear()
{
	char szTime[128];
	sprintf(szTime,"%02d.%02d %02d:%02d:%02d.%03d",
		tm_mon, tm_mday, tm_hour, tm_min, tm_sec, tm_millisec);

	return szTime;
}

std::string DateTime::ToDuration()
{
	char szTime[128];
	int nday = m_sec/24/3600;
	int nhour = m_sec/3600 %24;
	int nmin = m_sec/60 %60;
	int nsec = m_sec %60;
	if (nday>0)
		sprintf(szTime,"%dD%02d:%02d:%02d.%03d", nday, nhour, nmin, nsec, tm_millisec);
	else if(nhour>0)
		sprintf(szTime,"%d:%02d:%02d.%03d", nhour, nmin, nsec, tm_millisec);
	else if(nmin>0)
		sprintf(szTime,"%d:%02d.%03d", nmin, nsec, tm_millisec);
	else
		sprintf(szTime,"%d.%03d", nsec, tm_millisec);

	return szTime;
}

DateTime DateTime::Now()
{
#ifdef WIN32
    struct _timeb tb;
    _ftime(&tb);
#else		// no windows
    struct timeb tb;
	ftime(&tb);
#endif
	DateTime dt(tb.time);
	dt.tm_millisec = tb.millitm;
	dt.GetStructTime();	// getlocaltime, deal with timezone
	return dt;
}
//  = (tm_yday-1)*24*3600 + tm_hour*3600 + tm_min*60 + tm_sec
unsigned long DateTime::ToSeconds()
{
	return (tm_yday)*24*3600 + tm_hour*3600 + tm_min*60 + tm_sec;
}

// = 1000*(tm_hour*3600 + tm_min*60 + tm_sec) + tm_millisec // +tm_yday shall overflow
unsigned long DateTime::ToMilliSeconds()
{
	return 1000*(tm_hour*3600 + tm_min*60 + tm_sec) + tm_millisec;
}

} //namespace DHT
