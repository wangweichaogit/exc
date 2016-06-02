/************************************************************************/
/*
This file supply some functions to get system cpu usage,memory usage,
disk usage,support Windows,Linux.

hezhen@channelsoft.com
*/
/************************************************************************/
#ifndef __SYSTEM_STAT_H__
#define __SYSTEM_STAT_H__

#include "comLib/Logger.h"
#include "comLib/Mutex.h"

#ifdef WIN32
#include <Iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib")
#define SystemBasicInformation       0
#define SystemPerformanceInformation 2
#define SystemTimeInformation        3
#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))
typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
#else
typedef struct SysCpuTime
{
	unsigned long long int user;
	unsigned long long int nice;
	unsigned long long int system;
	unsigned long long int idle;
	unsigned long long int iowait;
	unsigned long long int irq;
	unsigned long long int softirq;
}SysCpuTime;
#endif

namespace DHT
{

class COMLIBAPI SystemStat
{
public:
	SystemStat();
	virtual ~SystemStat();

public:
	//获取系统当前的CPU使用率
	int get_cpu_usage();

	//获取系统当前的内存使用率
	int get_memory_usage(unsigned long *dwTotalPhysMem,unsigned long *dwAvailPhys,unsigned long *dwTotalVirtual,unsigned long *dwAvailVirtual);

	//获取当前磁盘使用,单位M
	int get_disk_usage(int *nTotalSpace,int *nFreeSpace);

	//获取第k块网卡MAC地址,如果k大于实际的网卡数，将返回最接近k的序号的网卡地址
	std::string get_mac_address(int k = 0,bool bExcludeLoopBack = true,const std::string &strIp = "");

private:
    Mutex m_mutex;
#ifdef WIN32
	PROCNTQSI NtQuerySystemInformation;
	LARGE_INTEGER m_liOldIdleTime;
	LARGE_INTEGER m_liOldSystemTime;
#else
	SysCpuTime m_sctOldTime;
	bool m_bFirst;
#endif
};

} // namespace DHT

#endif // #ifndef __SYSTEM_STAT_H__

