//
//  PCDefine.h
//  PCHotUpdate
//
//  Created by caoxin on 2017/3/30.
//  Copyright © 2017年 shanqiang. All rights reserved.
//

#ifndef PCDefine_h
#define PCDefine_h

#include <string>
#include <functional>
#ifdef WIN32
#include <windows.h>
//#include <wtypes.h>
//#include <OleAuto.h>
#else
#include <unistd.h>
#endif
#include <time.h>
#include <thread>
#include <vector>
#if defined(_MSC_VER) || defined(__MINGW32__)
#include <io.h>
#include "time.h"
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <list>
#include <mutex>
#define NAMESPACE_BEGIN  namespace gamebase {
#define NAMESPACE_END };

#define USING_NAMESPACE using namespace gamebase;

typedef std::function<void(void* ,int )> Callback;
typedef std::function<void(int ,double ,double )> Callback3;
typedef std::function<void(int ,int )> CallBack2;

// https://git.youle.game/TC/TAG/frontend/cocos2D/carbon/issues/46#note_14454 ，why segsize = 1024*256
static const int SEGMENT_SIZE = 1024*256;

#define PCDEBUG     0
#define PCINFO      1
#define PCWARNING   2
#define PCERROR     3

#define RETRYDOWNLOADS 15
#define REVDATATIMEOUT 5L

enum kError {
    kMD5Error =1001,
};
#define PCErrno (errno+10000)


#endif /* PCDefine_h */
