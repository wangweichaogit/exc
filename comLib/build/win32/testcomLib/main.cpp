// testcomLib

#include "comLib/LogHelper.h"
#include <string>

int main(int argc, char* argv[]) 
{
	int is_continue = 1;

	InitLogCfg("d:\\","test", CCLOG_DEBUG);

	for (int i=0; i<100; i++) {
		WriteLogD("This is comLib test app: is_continue:%d, row:%d", is_continue, i);
	}

	UninitLogCfg();

	return 0;
}
