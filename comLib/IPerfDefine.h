#ifndef  _DHT_DEFINE_H_
#define  _DHT_DEFINE_H_
#include <cstring>
namespace DHT
{

typedef void (*DHTPrintLogEntryCallback)(void *context,int nLogLevel,const char *szFormattedText);

typedef enum DHTLogMode
{
    DHT_NOT_WRITE_LOG = 0,
    DHT_ONLY_WRITE_LOG = 1,
    DHT_APP_WRITE_LOG = 2,
    DHT_BOTH_WRITE_LOG = 3
}DHTLogMode;

typedef struct DHTLogConfigure
{
    DHTLogConfigure()
    {
        dhn_log_mode = DHT_ONLY_WRITE_LOG;
        log_level = 0;
        log_context = NULL;
        pfnLogCallback = NULL;
        memset(log_path,0,sizeof(log_path));
        roll_volumes = 0;
        volume_size = 1024 * 10; //10M
        memset(prefix,0,sizeof(prefix));
    }

    DHTLogMode dhn_log_mode;
    int log_level;
    void *log_context;
    DHTPrintLogEntryCallback pfnLogCallback;
    char log_path[64];
    int roll_volumes;
    int volume_size;
    char prefix[64];
}DHTLogConfigure;
}
#endif//__DLLEXPORTORIMPORTDEFINE_H__
