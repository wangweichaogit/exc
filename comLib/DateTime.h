// DateTime.h: interface for the DateTime class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATETIME_H__045114A1_C6B3_429B_B2A4_A306D5F641E7__INCLUDED_)
#define AFX_DATETIME_H__045114A1_C6B3_429B_B2A4_A306D5F641E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
#include "comLib/Stdafx.h"

namespace DHT
{
class COMLIBAPI DateTime  
{
public:
	static DateTime Now();//MM.DD HH:MI:SS.mmm
	std::string ToString();//YYYY.MM.DD HH:MI:SS.mmm  2008.08.08 20:00:00.123
	std::string ToStringNoYear();//MM.DD HH:MI:SS.mmm  08.08 20:00:00.123
	std::string ToDuration();
	unsigned long ToSeconds();//  = (tm_yday-1)*24*3600 + tm_hour*3600 + tm_min*60 + tm_sec
	unsigned long ToMilliSeconds();// = 1000*(tm_hour*3600 + tm_min*60 + tm_sec) + tm_millisec // +tm_yday shall overflow
	//unsigned long ToMicroSeconds();//
	DateTime();
	DateTime(long time_t); //seconds ,,Milliseconds=0
	DateTime(long time_t, int millitm); //seconds ,,Milliseconds=0
	virtual ~DateTime();
	std::string ToStringForCdr();
	std::string ToStringForUpdate();

	int tm_millisec; //Milliseconds
    int tm_sec;     /* seconds after the minute - [0,59] */
    int tm_min;     /* minutes after the hour - [0,59] */
    int tm_hour;    /* hours since midnight - [0,23] */
    int tm_mday;    /* day of the month - [1,31] */
    int tm_mon;     /* months since January - [1,12] */
    int tm_year;    /* years  1970 -- 2038 */
    int tm_wday;    /* days since Sunday - [0,6]  Sunday=0*/
    int tm_yday;    /* days since January 1 - [0,365] */

	int GetSecond() const
	{
		return m_sec;
	}
	int GetMilliSecond() const
	{
		int nTemp = m_sec%1000000;
		return nTemp*1000+tm_millisec ;
	}

    DateTime operator-() const
    {
        return DateTime(-m_sec);
    }

    DateTime operator-(const DateTime& rhs) const
    {
		if (tm_millisec>=rhs.tm_millisec)
			return DateTime(m_sec - rhs.m_sec , tm_millisec-rhs.tm_millisec);
		else
			return DateTime(m_sec - rhs.m_sec-1 , 1000+tm_millisec-rhs.tm_millisec);
    }

    DateTime& operator-=(const DateTime& rhs)
    {
		if (tm_millisec>=rhs.tm_millisec)
		{
			m_sec -= rhs.m_sec;
			tm_millisec -= rhs.tm_millisec;
		}
		else
		{
			m_sec = m_sec - rhs.m_sec-1;
			tm_millisec = tm_millisec - rhs.tm_millisec +1000 ;
		}

        return *this;
    }

    DateTime operator+(const DateTime& rhs) const
    {
 		if (tm_millisec+rhs.tm_millisec<1000)
			return DateTime(m_sec + rhs.m_sec, tm_millisec+rhs.tm_millisec);
		else
			return DateTime(m_sec + rhs.m_sec+1, tm_millisec+rhs.tm_millisec-1000);
    }

    DateTime& operator+=(const DateTime& rhs)
    {
		if (tm_millisec+rhs.tm_millisec<1000)
		{
			m_sec += rhs.m_sec;
			tm_millisec += rhs.tm_millisec;
		}
		else
		{
			m_sec = m_sec + rhs.m_sec+1;
			tm_millisec = tm_millisec + rhs.tm_millisec -1000 ;
		}

        return *this;
    }

    bool operator<(const DateTime& rhs) const
    {
        return m_sec < rhs.m_sec;
    }

    bool operator<=(const DateTime& rhs) const
    {
        return m_sec <= rhs.m_sec;
    }

    bool operator>(const DateTime& rhs) const
    {
        return m_sec > rhs.m_sec;
    }

    bool operator>=(const DateTime& rhs) const
    {
        return m_sec >= rhs.m_sec;
    }

    bool operator==(const DateTime& rhs) const
    {
        return m_sec == rhs.m_sec;
    }

    bool operator!=(const DateTime& rhs) const
    {
        return m_sec != rhs.m_sec;
    }
private:
	void tmInit();
	void GetStructTime();
	//unsigned long m_sec; // seconds since 1970
	time_t m_sec;
};

} //namespace VOIP
#endif // !defined(AFX_DATETIME_H__045114A1_C6B3_429B_B2A4_A306D5F641E7__INCLUDED_)
