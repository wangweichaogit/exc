#ifndef __DHN_TIMER_CPP__
#define __DHN_TIMER_CPP__

#include "comLib/dhntimer/DHNTimer.h"
#include "comLib/dhntimer/DHNTimerWheel.h"

namespace DHT
{

/////////////////////////////////DHNTimer/////////////////////////////////////////
DHNTimer::DHNTimer(const std::string &strName)
{
	DHN_Free_List<DHN_Timer_Node_T<DHNTimerTask*> > *pTimerList = new DHN_Locked_Free_List<DHN_Timer_Node_T<DHNTimerTask*> >;
	TimerQueue<DHNTimerTask*> *pTimerQueue = new TimerWheel<DHNTimerTask*>(1024,100,0,pTimerList);
	m_intern_timer = new Timer<DHNTimerTask*>(pTimerQueue,strName);
}

DHNTimer::~DHNTimer()
{
	if (m_intern_timer != NULL)
	{
		delete m_intern_timer;
	}
}

bool DHNTimer::startTimer()
{
	if (m_intern_timer == NULL)
	{
		return false;
	}
	else
	{
		return m_intern_timer->startTimer();
	}
}

bool DHNTimer::stopTimer()
{
	if (m_intern_timer == NULL)
	{
		return false;
	}
	else
	{
		return m_intern_timer->stopTimer();
	}
}

bool DHNTimer::synStartTimer(void* lpParam,int nWaitTime)
{
	if (m_intern_timer == NULL)
	{
		return false;
	}
	else
	{
		return m_intern_timer->SynStart(lpParam,nWaitTime);
	}
}

bool DHNTimer::synStopTimer(int nWaitTime)
{
	if (m_intern_timer == NULL)
	{
		return false;
	}
	else
	{
		return m_intern_timer->SynStop(nWaitTime);
	}
}

int DHNTimer::addTimer(DHNTimerTask *pTask, int nTimeOut, void * arg1, void *arg2, bool bIsRepet)
{
	if (m_intern_timer == NULL)
	{
		return -1;
	}
	else
	{
		return m_intern_timer->addTimer(pTask,nTimeOut,arg1,arg2,bIsRepet);
	}
}

void DHNTimer::cancelTimer(int nTimerID,const void** act)
{
	if (m_intern_timer == NULL)
	{
		return;
	}
	else
	{
		m_intern_timer->cancelTimer(nTimerID,act);
	}
}

};

#endif
