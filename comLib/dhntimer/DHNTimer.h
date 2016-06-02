#ifndef __DHN_TIMER_H__
#define __DHN_TIMER_H__

#include "comLib/dhntimer/DHNTimer_T.h"

namespace DHT
{
class COMLIBAPI DHNTimer
{
public:
	DHNTimer(const std::string &strName = "");
	virtual ~DHNTimer();

public:
	bool	startTimer();
	bool	stopTimer();
	bool	synStartTimer(void* lpParam = 0,int nWaitTime = INFINITE);
	bool	synStopTimer(int nWaitTime = INFINITE);
	int		addTimer(DHNTimerTask *pTask, int nTimeOut, void * arg1,void *arg2, bool bIsRepet = false);
	void	cancelTimer(int nTimerID,const void** act = 0);
	bool	is_timer_stopped(){return m_intern_timer?m_intern_timer->ThreadHasEnd():true;}
	bool	is_timer_started(){return m_intern_timer?m_intern_timer->isStarted():false;}

private:
	Timer<DHNTimerTask*> *m_intern_timer;
};

};

#endif
