#include "SystemStat.h"

#ifdef WIN32
typedef struct
{
	DWORD   dwUnknown1;
	ULONG   uKeMaximumIncrement;
	ULONG   uPageSize;
	ULONG   uMmNumberOfPhysicalPages;
	ULONG   uMmLowestPhysicalPage;
	ULONG   uMmHighestPhysicalPage;
	ULONG   uAllocationGranularity;
	PVOID   pLowestUserAddress;
	PVOID   pMmHighestUserAddress;
	ULONG   uKeActiveProcessors;
	BYTE    bKeNumberProcessors;
	BYTE    bUnknown2;
	WORD    wUnknown3;
} SYSTEM_BASIC_INFORMATION;

typedef struct
{
	LARGE_INTEGER   liIdleTime;
	DWORD           dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct
{
	LARGE_INTEGER liKeBootTime;
	LARGE_INTEGER liKeSystemTime;
	LARGE_INTEGER liExpTimeZoneBias;
	ULONG         uCurrentTimeZoneId;
	DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;
#else
#ifndef __IOS__
#include <sys/vfs.h>
#endif
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#endif

#ifdef _ANDROID_
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#endif

namespace DHT
{

/////////////////////////////////SystemStat/////////////////////////////////////////
SystemStat::SystemStat()
{
#ifdef WIN32
	m_liOldIdleTime.QuadPart = 0;
	m_liOldSystemTime.QuadPart = 0;
#else
	memset(&m_sctOldTime,0,sizeof(SysCpuTime));
	m_bFirst = false;
#endif
}

SystemStat::~SystemStat()
{
}

int SystemStat::get_cpu_usage()
{
	DHT::SingleMutex s(m_mutex);

#ifdef WIN32
	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
	SYSTEM_TIME_INFORMATION        SysTimeInfo;
	SYSTEM_BASIC_INFORMATION       SysBaseInfo;
	double                         dbIdleTime;
	double                         dbSystemTime;
	LONG                           status;
	int UsageCpu = 0;

	NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(
		GetModuleHandle(L"ntdll"),
		"NtQuerySystemInformation"
		);

	if (!NtQuerySystemInformation)
		return 0;

	status = NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SysBaseInfo),NULL);
	if (status != NO_ERROR)
		return 0;


	status = NtQuerySystemInformation(SystemTimeInformation,&SysTimeInfo,sizeof(SysTimeInfo),0);
	if (status!=NO_ERROR)
		return 0;

	status = NtQuerySystemInformation(SystemPerformanceInformation,&SysPerfInfo,sizeof(SysPerfInfo),NULL);
	if (status != NO_ERROR)
		return 0;

	if (m_liOldIdleTime.QuadPart != 0)
	{
		dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(m_liOldIdleTime);
		dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(m_liOldSystemTime);

		dbIdleTime = dbIdleTime / dbSystemTime;


		dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors + 0.5;
		UsageCpu = (int)dbIdleTime;

	}
	else
	{
		m_liOldIdleTime = SysPerfInfo.liIdleTime;
		m_liOldSystemTime = SysTimeInfo.liKeSystemTime;
		return 0;
	}

	// store new CPU's idle and system time
	m_liOldIdleTime = SysPerfInfo.liIdleTime;
	m_liOldSystemTime = SysTimeInfo.liKeSystemTime;

	if (UsageCpu < 0)
	{
		UsageCpu = 0;
	}

	if (UsageCpu > 100)
	{
		UsageCpu = 100;
	}
	return UsageCpu;
#elif defined (_REDHAT_) || defined (_ANDROID_)
	char cpu[8] = {0};
	char text[1024] = {0};
	FILE *fp = fopen("/proc/stat", "r");
	if (fp == NULL)
	{
		return -1;
	}
	fread(text, 1, sizeof(text), fp);
	SysCpuTime sct;
	if (strstr(text, "cpu"))
	{
		sscanf(text, "%s %llu %llu %llu %llu %llu %llu %llu", 
			cpu,
			&sct.user,
			&sct.nice,
			&sct.system,
			&sct.idle,
			&sct.iowait,
			&sct.irq,
			&sct.softirq);
	}
	fclose(fp);

	printf("%llu %llu %llu %llu %llu %llu %llu\n",			
		sct.user,
		sct.nice,
		sct.system,
		sct.idle,
		sct.iowait,
		sct.irq,
		sct.softirq);

	int nUsage = 0;
	if (m_bFirst)
	{
		m_bFirst = false;
		memcpy(&m_sctOldTime,&sct,sizeof(SysCpuTime));
	}
	else
	{
		unsigned long long int current_total = 
			sct.user +
			sct.nice +
			sct.system +
			sct.idle +
			sct.iowait +
			sct.irq +
			sct.softirq;

		unsigned long long int last_total = 
			m_sctOldTime.user +
			m_sctOldTime.nice +
			m_sctOldTime.system +
			m_sctOldTime.idle +
			m_sctOldTime.iowait +
			m_sctOldTime.irq +
			m_sctOldTime.softirq;

		unsigned long long int total = current_total - last_total;
		if (total != 0)
		{
			unsigned long long int idle = sct.idle - m_sctOldTime.idle;
			nUsage = 100 * (total - idle)/total;
		}

		memcpy(&m_sctOldTime,&sct,sizeof(SysCpuTime));
	}
	return nUsage;
#else
	return -1;
#endif
}

int SystemStat::get_memory_usage(unsigned long *dwTotalPhysMem,unsigned long *dwAvailPhys,unsigned long *dwTotalVirtual,unsigned long *dwAvailVirtual)
{
#ifdef WIN32
	MEMORYSTATUS memStatus;
	GlobalMemoryStatus(&memStatus);
	*dwTotalPhysMem = memStatus.dwTotalPhys/1024;
	*dwAvailPhys = memStatus.dwAvailPhys/1024;
	*dwTotalVirtual = memStatus.dwTotalVirtual/1024;
	*dwAvailVirtual = memStatus.dwAvailVirtual/1024;
	return 0;
#elif defined (_REDHAT_) || defined (_ANDROID_)
	FILE *fp = fopen("/proc/meminfo", "r");
	if (fp == NULL)
	{
		return -1;
	}

	char total[256] = {0};
	char free[256] = {0};
	char vtotal[256] = {0};
	char vfree[256] = {0};

	char MemTotal[32] = {0};
	char MemFree[32] = {0};
	char VMemTotal[32] = {0};
	char VMemFree[32] = {0};

	char line[256] = {0};

	while(!feof(fp))
	{
		fgets(line,sizeof(line)-1,fp);
		if (strstr(line,"MemTotal") != NULL)
		{
			strncpy(total,line,255);
		}
		else if (strstr(line,"MemFree") != NULL)
		{
			strncpy(free,line,255);
		}
		else if (strstr(line,"VmallocTotal"))
		{
			strncpy(vtotal,line,255);
		}
		else if(strstr(line,"VmallocUsed"))
		{
			strncpy(vfree,line,255);
		}
		memset(line,0,sizeof(line));
	}

	fclose(fp);

	int i,j;
	for(i = 0,j = 0;i < (int)strlen(total);i++)
	{
		if(isdigit(total[i]))
		{
			MemTotal[j++] = total[i];
		} 
	}

	for(i=0,j=0;i<(int)strlen(free);i++)
	{
		if(isdigit(free[i]))
		{
			MemFree[j++] = free[i];
		} 
	}

	for(i=0,j=0;i<(int)strlen(vtotal);i++)
	{
		if(isdigit(vtotal[i]))
		{
			VMemTotal[j++]=vtotal[i];
		} 
	}

	for(i=0,j=0;i<(int)strlen(vfree);i++)
	{
		if(isdigit(vfree[i]))
		{
			VMemFree[j++] = vfree[i];
		} 
	}

	*dwTotalPhysMem = strtoul(MemTotal,0,10);
	*dwAvailPhys = strtoul(MemFree,0,10);
	*dwTotalVirtual = strtoul(VMemTotal,0,10);
	*dwAvailVirtual = strtoul(VMemFree,0,10);

	return 0;
#else
	return -1;
#endif
}

int SystemStat::get_disk_usage(int *nTotalSpace,int *nFreeSpace)
{
#ifdef WIN32
	*nTotalSpace = 0;
	*nFreeSpace = 0;
	ULARGE_INTEGER FreeAvailable,TotalNum,TotalFreeNum;

	WCHAR p[3] = {0};
	bool b_flag;

	//得到有效的驱动器名,即盘符 
	for( int drive = 1; drive <= 26; drive++ )
	{
		if( !_chdrive( drive ) )
		{
			memset( p , 0 , sizeof(p));
			p[0] = drive + 'A' - 1;
			p[1] = ':';
			p[2] = '\0';
			b_flag = GetDiskFreeSpaceEx(p ,&FreeAvailable,&TotalNum,&TotalFreeNum );
			if( b_flag )
			{
				*nTotalSpace += (int)(TotalNum.QuadPart/(1024*1024));
				*nFreeSpace += (int)(FreeAvailable.QuadPart/(1024*1024));
			}
		}
 	}
	return 0;
#elif defined _REDHAT_
	long long free_space = 0;
	long long total_space = 0;
	struct statfs buf;

	statfs("/", &buf);

	free_space = (long long)buf.f_bavail*(long long)buf.f_bsize;
	total_space = (long long)buf.f_blocks*(long long)buf.f_bsize;
	
	*nTotalSpace = total_space/(1024 * 1024);
	*nFreeSpace = free_space/(1024 * 1024);
	return 0;
#else
	return -1;
#endif
}

std::string SystemStat::get_mac_address(int k,bool bExcludeLoopBack,const std::string &strIp)
{
	std::string strMacAddress = "000000000000";
#ifdef WIN32
	bool bCheckIp = false;
	if (!strIp.empty())
	{
		bCheckIp = true;
	}
	//PIP_ADAPTER_INFO结构体指针存储本机网卡信息
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	//得到结构体大小,用于GetAdaptersInfo参数
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	//调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
	int nRel = GetAdaptersInfo(pIpAdapterInfo,&stSize);
	if (ERROR_BUFFER_OVERFLOW==nRel)
	{
		//如果函数返回的是ERROR_BUFFER_OVERFLOW则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
		//这也是说明为什么stSize既是一个输入量也是一个输出量
		//释放原来的内存空间
		delete pIpAdapterInfo;
		//重新申请内存空间用来存储所有网卡信息
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
		nRel=GetAdaptersInfo(pIpAdapterInfo,&stSize);
	}

	if (ERROR_SUCCESS==nRel)
	{
		PIP_ADAPTER_INFO p = pIpAdapterInfo;
		PIP_ADAPTER_INFO selected = NULL;
		int i = 0;
		do
		{
			if (p == NULL)
			{
				break;
			}

			if (bExcludeLoopBack)
			{
				if (p->Type != MIB_IF_TYPE_LOOPBACK && strstr(p->Description,"Loopback") == NULL)
				{
					if (strcmp(p->IpAddressList.IpAddress.String,"0.0.0.0") != 0)
					{
						if (bCheckIp) 
						{
							if (strcmp(p->IpAddressList.IpAddress.String,strIp.c_str()) == 0)
							{
								selected = p;
							}
						}
						else
						{
							selected = p;
						}
					}
				}
			}
			else
			{
				if (strcmp(p->IpAddressList.IpAddress.String,"0.0.0.0") != 0)
				{
					if (bCheckIp) 
					{
						if (strcmp(p->IpAddressList.IpAddress.String,strIp.c_str()) == 0)
						{
							selected = p;
						}
					}
					else
					{
						selected = p;
					}
				}
			}

			if (i >= k && selected != NULL)
			{
				break;
			}

			i++;
			p = p->Next;
		}
		while(p);

		if (selected)
		{
			char szMac[64] = {0};
			for (UINT i = 0; i < selected->AddressLength; i++)
			{
				char item[8] = {0};
				sprintf(item,"%02x",selected->Address[i]);
				strcat(szMac,item);
			}
			strMacAddress = szMac;
		}
	}

	//释放内存空间
	if (pIpAdapterInfo)
	{
		delete pIpAdapterInfo;
	}
#elif defined _REDHAT_
	int nSockFd = 0;
	do 
	{
		nSockFd = socket(AF_INET, SOCK_STREAM, 0);
		if (nSockFd < 0)
		{
			break;
		}

		struct ifreq tmp;
		memset(&tmp,0,sizeof(struct ifreq));
		strncpy(tmp.ifr_name,"eth0",sizeof(tmp.ifr_name)-1);
		if( (ioctl(nSockFd,SIOCGIFHWADDR,&tmp)) < 0 )
		{
			break;
		}

		char mac_addr[32] = {0};
		sprintf(mac_addr, "%02x%02x%02x%02x%02x%02x",
			(unsigned char)tmp.ifr_hwaddr.sa_data[0],
			(unsigned char)tmp.ifr_hwaddr.sa_data[1],
			(unsigned char)tmp.ifr_hwaddr.sa_data[2],
			(unsigned char)tmp.ifr_hwaddr.sa_data[3],
			(unsigned char)tmp.ifr_hwaddr.sa_data[4],
			(unsigned char)tmp.ifr_hwaddr.sa_data[5]);
		strMacAddress = mac_addr;
	} while (0);

	if (nSockFd > 0)
	{
		close(nSockFd);
	}
#elif defined _ANDROID_
	struct ifreq *ifr = NULL;  
	struct ifconf ifc;
	int sock = 0;

	// find number of interfaces.  
	memset(&ifc, 0, sizeof(ifc));  
	ifc.ifc_ifcu.ifcu_req = NULL;  
	ifc.ifc_len = 0;
	do 
	{
		sock = socket(PF_INET, SOCK_STREAM, 0);
		if (sock < 0)
		{
			break;
		}

		if (ioctl(sock, SIOCGIFCONF, &ifc) < 0)
		{
			break;
		}

		ifr = (ifreq*) malloc(ifc.ifc_len);
		ifc.ifc_ifcu.ifcu_req = ifr;

		if (ioctl(sock, SIOCGIFCONF, &ifc) < 0)
		{
			break;
		}

		int nIfNumber = ifc.ifc_len / sizeof(struct ifreq);

		for (int i = 0; i < nIfNumber; i++)  
		{  
			struct ifreq *r = &ifr[i];  
			struct sockaddr_in *sin = (struct sockaddr_in*)&r->ifr_addr;  
			if (strcmp(r->ifr_name, "lo") == 0)
			{
				continue; // skip loopback interface  
			}

			// get MAC address  
			if(ioctl(sock, SIOCGIFHWADDR, r) < 0)  
			{
				continue;  
			}  

			if (r->ifr_hwaddr.sa_data == NULL)
			{
				continue;
			}

			char mac_addr[32] = {0};
			sprintf(mac_addr, "%02x%02x%02x%02x%02x%02x",
				(unsigned char)r->ifr_hwaddr.sa_data[1],
				(unsigned char)r->ifr_hwaddr.sa_data[0],
				(unsigned char)r->ifr_hwaddr.sa_data[2],
				(unsigned char)r->ifr_hwaddr.sa_data[3],
				(unsigned char)r->ifr_hwaddr.sa_data[4],
				(unsigned char)r->ifr_hwaddr.sa_data[5]);

			strMacAddress = mac_addr;
			break;
		}  
	} while (0);

	if (sock != 0)
	{
		close(sock);
	}

	if (ifr != NULL)
	{
		free(ifr);
	}
#else
#endif
	return strMacAddress;
}

} //namespace DHT
