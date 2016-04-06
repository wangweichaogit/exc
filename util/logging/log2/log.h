#ifndef __LOG_H_
#define __LOG_H_

#include <string>
#include <stdint.h>
#include <stdlib.h>
#include <boost/filesystem/path.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/once.hpp>
#include <boost/thread/thread.hpp>
#include <stdio.h>


int LogInfoStr(const std::string &str);
#define LogPrint(...) LogInfoStr(__VA_ARGS__)
static inline bool error(const char* format)
{
    LogInfoStr(std::string("ERROR: ") + format + "\n");
    return false;
}
#endif