#ifndef __DHN_TIMER_TASK_H__
#define __DHN_TIMER_TASK_H__

#include "comLib/dhntimer/DHNTimerDefine.h"
#include "comLib/dhntimer/Time_Value.h"

namespace DHT
{

class  COMLIBAPI DHNTimerTask
{
public:
	DHNTimerTask();
	virtual ~DHNTimerTask();

public:
	void stop_repeated(){m_bStopRepeated = true;}
	bool is_stop_repeated(){return m_bStopRepeated;}

public:
	virtual int handle_timeout(int nTimerId,const TimeValue &tv,const void *arg1,const void *arg2) = 0;
	virtual int handle_repeated_timer_stoped();

private:
	bool m_bStopRepeated;
};

};


#endif

