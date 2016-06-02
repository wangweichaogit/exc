#ifndef __DHN_TIMER_TASK_CPP__
#define __DHN_TIMER_TASK_CPP__

#include "comLib/dhntimer/DHNTimerTask.h"
//#include "comLib/DateTime.h"

namespace DHT
{

DHNTimerTask::DHNTimerTask():
m_bStopRepeated(false)
{

}

DHNTimerTask::~DHNTimerTask()
{

}

int DHNTimerTask::handle_repeated_timer_stoped()
{
	delete this;
	return 0;
}

};


#endif
