/*//
//  C7z.cpp
//  testLzma
//
//  Created by shanqiang on 15/3/8.
//  Copyright (c) 2015年 shanqiang. All rights reserved.
//
*/
#include "C7z.h"

#ifdef WIN32
#include <windows.h>

#else
#include <unistd.h>
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>


int C7z::extract(const char * srcPath, const char * dstPath, Progress7z progress7z) {
    
#ifdef WIN32
	CreateDirectoryA(dstPath, NULL);
#else
	mkdir(dstPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif // WIN32

    char *args[4];
    args[0] = (char *)"7zDec";
    args[1] = (char *)"x";
    args[2] = (char *)srcPath;
    args[3] = (char *)dstPath;
	int ret = main7z(sizeof(args) / sizeof(args[0]), args, progress7z);
	return ret;
}

bool C7z::testIntegrity(const char * srcPath, Progress7z progress7z) {
    char *args[3];
    args[0] = (char *)"7zDec";
    args[1] = (char *)"t";
    args[2] = (char *)srcPath;
    
    if (0 == main7z(3, args, progress7z)) {
        return true;
    }
    
    return false;
}


void C7z::test() {
    char *args[3];
    args[0] = (char *)"7zDec";
    args[1] = (char *)"t";
    args[2] = (char *)"/tmp/test.7z";
    main7z(3, args, nullptr);
    
    char cwd[1024];

#ifdef WIN32
	GetCurrentDirectoryA(sizeof(cwd), cwd);
	SetCurrentDirectoryA("/tmp/test7zx");
	GetCurrentDirectoryA(sizeof(cwd), cwd);
#else
	getcwd(cwd, sizeof(cwd));
	chdir("/tmp/test7zx");
	getcwd(cwd, sizeof(cwd));
#endif // WIN32

    args[0] = (char *)"7zDec";
    args[1] = (char *)"x";
    args[2] = (char *)"/tmp/test.7z";
    main7z(3, args, nullptr);
}
