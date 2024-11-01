//
//  C7z.h
//  testLzma
//
//  Created by shanqiang on 15/3/8.
//  Copyright (c) 2015年 shanqiang. All rights reserved.
//

#ifndef __testLzma__C7z__
#define __testLzma__C7z__

#include "7zMain.h"

class C7z {
public:
    void test();
    int extract(const char * srcPath, const char * dstPath, Progress7z progress7z);
    bool testIntegrity(const char * srcPath, Progress7z progress7z);
};

#endif /* defined(__testLzma__C7z__) */
