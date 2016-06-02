#ifndef __DHN_TIMER_T_H__
#define __DHN_TIMER_T_H__

#include "comLib/dhntimer/SocketDefine.h"
#include "comLib/Thread.h"
#include "comLib/Logger.h"
#include "comLib/dhntimer/DHNTimerQueue.h"
#include "comLib/dhntimer/DHNTimerDefine.h"
#include "comLib/dhntimer/DHNTimerTask.h"

namespace DHT
{
// class DHNTimerTask;

typedef TimerQueue<DHNTimerTask*> DHN_Timer_Queue;

template <class TYPE>
class Timer : public Thread
{
public:
	Timer(DHN_Timer_Queue *tq,const std::string &strName = "");
	virtual ~Timer();

public:
	bool	startTimer();
	bool	stopTimer();
	bool	synStopTimer(int nWaitTime = INFINITE);
	int		addTimer(TYPE type, int nTimeOut, void * arg1, void * arg2, bool bIsRepet);
	void	cancelTimer(int nTimerID,const void** act = 0);
	virtual int open (size_t max_number_of_handles = 256,
		bool restart = false,
		DHN_Timer_Queue * = 0);
	bool	isStarted();

protected:
	virtual void run(void* lpParam);
	int init_socket(char *szIp,unsigned short &usPort);


protected:
	TimerQueue<TYPE> *m_pTimerQueue;
	bool initialized_;
	bool restart_;
	bool delete_timer_queue_;
	fd_set m_read_fd_set;
	int m_nSocket;
    // DHT::Logger m_logger;
};

/////////////////////////////////Timer/////////////////////////////////////////
template <class TYPE>
Timer<TYPE>::Timer(DHN_Timer_Queue *tq,const std::string &strName):m_pTimerQueue(NULL),initialized_(false)
{
	FD_ZERO(&m_read_fd_set);
	this->open (256,0,tq);
	if (strName.empty())
	{
		// m_logger.SetTag("Timer");
	}
	else
	{
		std::string strInnerName = strName + "_Timer";
		// m_logger.SetTag(strInnerName.c_str());
	}
}

template <class TYPE>
Timer<TYPE>::~Timer()
{
	if (m_pTimerQueue != NULL)
	{
		delete m_pTimerQueue;
	}
}

template <class TYPE> 
int Timer<TYPE>::open(size_t size,
	bool restart,
	DHN_Timer_Queue *tq)
{
	if (this->initialized_)
		return -1;

	this->restart_ = restart;
	this->m_pTimerQueue = tq;
	int result = 0;
	this->initialized_ = true;
	return result;
}

template <class TYPE>
bool Timer<TYPE>::startTimer()
{
	return Start();
}

template <class TYPE>
bool Timer<TYPE>::stopTimer()
{
	return Stop();
}

template <class TYPE>
bool Timer<TYPE>::synStopTimer(int nWaitTime)
{
	return SynStop(nWaitTime);
}

template <class TYPE>
int Timer<TYPE>::addTimer(TYPE type, int nTimeOut, void * arg1, void *arg2, bool bIsRepet)
{
	int nRet = 0;
	while (1)
	{
		if (type == NULL)
		{
			nRet = -1;
			break;
		}

		if (m_pTimerQueue == NULL)
		{
			nRet = -2;
			break;
		}

		time_t nSecond = nTimeOut / 1000;
		long nUSecond = 1000 * (nTimeOut % 1000);
		TimeValue tv(nSecond,nUSecond);
		if (!bIsRepet)
		{
			nRet = m_pTimerQueue->schedule(type,arg1,arg2,tv);
		}
		else
		{
			nRet = m_pTimerQueue->schedule(type,arg1,arg2,tv,tv);
		}

		break;
	}

	// m_logger.Debug("[TimerId=%d] timer started,expires:%d,arg:%x",nRet,nTimeOut,arg);
	return nRet;
}

template <class TYPE>
void Timer<TYPE>::cancelTimer(int nTimerID,const void** act)
{
	if (m_pTimerQueue == NULL)
	{
		return;
	}

	m_pTimerQueue->cancel_timer(nTimerID,act);
	// m_logger.Debug("[TimerId=%d] timer canceled",nTimerID);
}

template <class TYPE>
int Timer<TYPE>::init_socket(char *szIp,unsigned short &usPort)
{
	int nRt = 0;

	while(1)
	{
		/*Allocate a socket fd*/
		m_nSocket = SOCKET_OPEN(AF_INET,SOCK_DGRAM,0);
		if (m_nSocket < 0)
		{			
			nRt = -1;
			break;
		}

		/*Unblock socket,tbd*/
		unsigned long ulSocketBlockOff = 1;
		if (SOCKET_UNBLOCK(m_nSocket,ulSocketBlockOff) < 0)
		{
#ifdef WIN32
			closesocket(m_nSocket);
#else
			close(m_nSocket);
#endif
			nRt = -2;
			break;
		}

		/*Padding addr struct*/
		struct sockaddr_in  ipSin;
		memset((char *)&ipSin, 0, sizeof(ipSin));
		ipSin.sin_family		= AF_INET;
		if (szIp == NULL || strlen(szIp) == 0 || strcmp(szIp,"0.0.0.0") == 0)
		{
			ipSin.sin_addr.s_addr = INADDR_ANY;
		}
		else
		{
			ipSin.sin_addr.s_addr = inet_addr(szIp);
		}

		ipSin.sin_port = htons(usPort);

		/*Binding local socket*/
		int nError = -1;
		nError = (SOCKET_BIND(m_nSocket, ipSin) != 0);
		if (nError)
		{
			int err = 0;
#ifdef WIN32
			err = WSAGetLastError();
#else
			err = errno;
#endif
			ipSin.sin_port = htons(0);
			nError = (SOCKET_BIND(m_nSocket, ipSin) != 0);
			if (nError)
			{
#ifdef WIN32
				closesocket(m_nSocket);
#else
				close(m_nSocket);
#endif
				nRt = -3;
				break;
			}
		}

		int  len = sizeof(ipSin);
		getsockname(m_nSocket, (struct sockaddr*) &ipSin, (socklen_t*)&len);
		usPort = ntohs(ipSin.sin_port);
		break;
	}

	return nRt;
}

template <class TYPE>
void Timer<TYPE>::run(void* lpParam)
{
	if (m_pTimerQueue == NULL)
	{
		return;
	}

	//m_bRunning = true;
	unsigned short nPort = 0;
	int nRet = init_socket(NULL,nPort);
	if (nRet < 0)
	{
		// m_logger.Debug("StartTimer init_socket failed");
		return;
	}
	// m_logger.Debug("Timer running");

	while(m_bRunning)
	{
		TimeValue *this_timeout = 0;
		TimeValue timer_buf (0);
		TimeValue max_wait_time;
		long msec_ = 300;
		max_wait_time.msec(msec_);
		this_timeout = this->m_pTimerQueue->calculate_timeout(&max_wait_time,&timer_buf);
		struct timeval tv;
		tv.tv_sec = (long)this_timeout->sec();
		tv.tv_usec = this_timeout->usec();

		_FD_ZERO(&m_read_fd_set);
		_FD_SET(m_nSocket,&m_read_fd_set);
		int nMax = m_nSocket + 1;
		//do 
		//{
			nRet = select(nMax,&m_read_fd_set,(fd_set*)0,(fd_set*)0,&tv);
			if(nRet < 0){
				// m_logger.Debug("DHNTimer_t timer socket fail socket start clesed .");
//#ifdef WIN32
//				closesocket(m_nSocket);
//#else
//				close(m_nSocket);
//#endif
				// m_logger.Debug("DHNTimer_t timer socket fail socket clesed  ok ~~~!!");
				break;
			}
		//} while (nRet < 0);

		m_pTimerQueue->expire();
	}

	if(m_nSocket > 0)
	{
#ifdef WIN32
	closesocket(m_nSocket);
#else
	close(m_nSocket);
#endif
	m_nSocket = -1;
	}
	// m_logger.Debug("Timer stopped");
}

template <class TYPE>
bool Timer<TYPE>::isStarted()
{
	return m_bRunning;
}
};

// #include "DHNTimer_T.cpp"

#endif
