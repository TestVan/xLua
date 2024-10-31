//
//  tjConfig.h
//  foundation
//
//  Created by 郭黎阳 on 2018/7/16.
//  Copyright © 2018年 topjoy. All rights reserved.
//
//  通用设置

#ifndef tjConfig_h
#define tjConfig_h

#include "string.h"

#if !defined(UNITY) && !defined(COCOS) && (defined(COCOS2D_DEBUG) || defined(CC_STATIC) || defined(COCOS2D_LUAOBJ_DEBUG) || defined(CC_TARGET_OS_MAC) || defined(CC_TARGET_OS_IPHONE) || defined(CC_TARGET_OS_TVOS))
#define COCOS
#endif

#if defined(COCOS)

#include "base/CCConsole.h"

#define TJLOG(format, ...)      CCLOG(format, ##__VA_ARGS__)

#elif defined(UNITY)

#include <stdio.h>
#if WIN32
    #include <windows.h>
#else
    #if ANDROID
        #include <time.h>
    #elif MACOS
        #include <sys/time.h>
    #else //linux下需要下面两个
        #include <sys/time.h>
        #include <time.h>
    #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
extern void UnityLog(const char* str, ...);
    
#if __cplusplus
}
#endif

#define TJLOG(format, ...)      UnityLog(format, ##__VA_ARGS__)

#else

#define TJLOG(format, ...)  printf(format "\n", ##__VA_ARGS__)

#endif

#define TJLOG_VERBOSE   0
#define TJLOG_DEBUG     1
#define TJLOG_INFO      2
#define TJLOG_WARNING   3
#define TJLOG_ERROR     4
#define TJLOG_DISABLED  5

#ifndef TJLOG_THREHOLD

#if defined(COCOS)

#if defined(COCOS2D_DEBUG) && COCOS2D_DEBUG > 0
#define TJLOG_THREHOLD  TJLOG_DEBUG
#else
#define TJLOG_THREHOLD  TJLOG_DISABLED
#endif

#elif defined(UNITY)

#if Debug
#define TJLOG_THREHOLD  TJLOG_INFO
#else
#define TJLOG_THREHOLD  TJLOG_WARNING
#endif

#else

#define TJLOG_THREHOLD  TJLOG_DISABLED

#endif /* COCOS */

#endif /* TJLOG_THREHOLD */

#if TJLOG_DISABLED <= TJLOG_THREHOLD
#undef TJLOG
#define TJLOG(...)
#endif

struct __TJ_DATE_STR__ {
	char str[16];
};

#ifdef _WIN32 
#define TJ_PATH_SEPARATOR   '\\'
inline struct __TJ_DATE_STR__ tj_get_date_str()
{
	__TJ_DATE_STR__ dateStr;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	_snprintf(dateStr.str, sizeof(dateStr.str), "%02hd:%02hd:%02hd.%03hd", wtm.wHour, wtm.wMinute, wtm.wSecond, wtm.wMilliseconds);
	return dateStr;
}
#else
#define TJ_PATH_SEPARATOR   '/'
inline struct __TJ_DATE_STR__ tj_get_date_str()
{
    __TJ_DATE_STR__ dateStr;
    struct timeval curTime;
    gettimeofday(&curTime, NULL);

    struct tm nowTime;
    localtime_r(&curTime.tv_sec, &nowTime);
    snprintf(dateStr.str, sizeof(dateStr.str), "%02d:%02d:%02d.%03d", (int)nowTime.tm_hour, (int)nowTime.tm_min, (int)nowTime.tm_sec, (int)curTime.tv_usec / 1000);
    return dateStr;
}
#endif

#define TJ_BASENAME(p)  (strrchr((p), TJ_PATH_SEPARATOR) ? strrchr((p), TJ_PATH_SEPARATOR) + 1 : (p))

#define _TJLOG(loglevel, fmt, ...)  TJLOG("[" #loglevel " %s %d %s %s] " fmt, TJ_BASENAME(__FILE__), __LINE__, __FUNCTION__, tj_get_date_str().str, ##__VA_ARGS__)

#if TJLOG_VERBOSE >= TJLOG_THREHOLD
#define TJVERBOSE_ENABLED
#define TJVERBOSE(fmt, ...)  _TJLOG(VERBOSE, fmt, ##__VA_ARGS__)
#else
#define TJVERBOSE(...)
#endif

#if TJLOG_DEBUG >= TJLOG_THREHOLD
#define TJDEBUG_ENABLED
#define TJDEBUG(fmt, ...)  _TJLOG(DEBUG, fmt, ##__VA_ARGS__)
#else
#define TJDEBUG(...)
#endif

#if TJLOG_INFO >= TJLOG_THREHOLD
#define TJINFO_ENABLED
#define TJINFO(fmt, ...)  _TJLOG(INFO, fmt, ##__VA_ARGS__)
#else
#define TJINFO(...)
#endif

#if TJLOG_WARNING >= TJLOG_THREHOLD
#define TJWARNING_ENABLED
#define TJWARNING(fmt, ...)  _TJLOG(WARNING, fmt, ##__VA_ARGS__)
#else
#define TJWARNING(...)
#endif

#if TJLOG_ERROR >= TJLOG_THREHOLD
#define TJERROR_ENABLED
#define TJERROR(fmt, ...)  _TJLOG(ERROR, fmt, ##__VA_ARGS__)
#else
#define TJERROR(...)
#endif

#if TJLOG_DISABLED <= TJLOG_THREHOLD
#define TJASSERT(cond, ...)
#else

#define TJASSERT_ENABLED

#ifdef COCOS

#define TJASSERT(cond, format, ...) do { \
    char msgBuf[128]; \
    snprintf(msgBuf, 128, "%s %d %s: " format, TJ_BASENAME(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    CCASSERT(cond, msgBuf); \
} while(0)

#else

#include <assert.h>
#define TJASSERT(cond, format, ...) do { \
    if (!(cond)) { \
        _TJLOG(ASSERT, format, ##__VA_ARGS__); \
        assert(0); \
    } \
} while(0)

#endif

#endif /* TJLOG_DISABLED <= TJLOG_THREHOLD */

#endif /* tjConfig_h */
