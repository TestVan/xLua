//
//  tjMacros.h
//  foundation
//
//  Created by 郭黎阳 on 2018/7/17.
//  Copyright © 2018年 topjoy. All rights reserved.
//
//  通用宏定义

#ifndef _TJMACROS_H_
#define _TJMACROS_H_

#include "tjConfig.h"

#ifdef __cplusplus
#define NS_GB_BEGIN                     namespace gamebase {
#define NS_GB_END                       }
#define USING_NS_GB                     using namespace gamebase
#define NS_GB                           ::gamebase
#else
#define NS_GB_BEGIN
#define NS_GB_END
#define USING_NS_GB
#define NS_GB
#endif

#define SET_FLAG(n, bit)    ((n) |= (1<<bit))
#define CLEAR_FLAG(n, bit)  ((n) &= (~(1<<bit)))
#define HAS_FLAG(n, bit)    ((n) & (1<<bit))

#endif /* _TJMACROS_H_ */
