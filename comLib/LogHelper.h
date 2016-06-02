//
//  LogHelper.h
//  LogHelper
//
//  Created by gongai on 14-10-27.
//  Copyright 2014 青牛(北京)技术有限公司. All rights reserved.
//

#ifndef __LOG_HELPER_H
#define __LOG_HELPER_H

#define CCLOG_ERROR 1
#define CCLOG_WARN  3
#define CCLOG_INFO  7
#define CCLOG_DEBUG 9

#ifdef WIN32
#ifdef COMLIB_EXPORTS
#define COMLIBAPI __declspec(dllexport)
#else
#define COMLIBAPI __declspec(dllimport)
#endif
#else
#define COMLIBAPI
#endif

#ifdef __cplusplus
    extern "C" {
#endif
        
/**
 *
 * Initialize log config
 *
 * logdir - log file directory
 * filename - log file name, no extension
 * loglevel - 1-ERROR, 3-WARN, 7-INFO, 9-DEBUG
 *
 */
void COMLIBAPI InitLogCfg(const char* logdir, const char* filename, int loglevel);
        
void COMLIBAPI UninitLogCfg();

void COMLIBAPI WriteLogD(const char* format, ...);
        
void COMLIBAPI WriteLogE(const char* format, ...);
        
void COMLIBAPI WriteLogW(const char* format, ...);
        
void COMLIBAPI WriteLogI(const char* format, ...);
        
#ifdef __cplusplus
    }
#endif

#endif /*__LOG_HELPER_H*/