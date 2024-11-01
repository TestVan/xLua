//
//  PCZipExtract.cpp
//  PCHotUpdate
//
//  Created by caoxin on 2017/4/7.
//  Copyright © 2017年 caoxin. All rights reserved.
//

#include "PCZipExtract.h"
#include "C7z.h"

USING_NAMESPACE
static PCZipExtract* __instance__ = nullptr;
PCZipExtract* PCZipExtract::getInstance()
{
    if(__instance__ == nullptr)
        __instance__ = new PCZipExtract();
    return __instance__;
}
void PCZipExtract::destoryInstance()
{
    if(__instance__)
        delete __instance__;
    __instance__ = nullptr;
}
PCZipExtract::PCZipExtract()
:_integrityTestCall(nullptr)
,_extractCall(nullptr)
,_extractThread(nullptr)
{
    
}

PCZipExtract::~PCZipExtract()
{
    destoryThread();
}
void PCZipExtract::destoryThread()
{
    if(_extractThread)
    {
        if(_extractThread->joinable())
            _extractThread->join();
        delete _extractThread;
        
    }
    _extractThread = nullptr;
}

/*
#ifdef WIN32
static std::string UTF8ToANSI(std::string s)
{
    BSTR    bstrWide;
    char*   pszAnsi;
    int     nLength;
    const char *pszCode = s.c_str();

    nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, NULL, NULL);
    bstrWide = SysAllocStringLen(NULL, nLength);

    MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, bstrWide, nLength);

    nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
    pszAnsi = new char[nLength];

    WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
    SysFreeString(bstrWide);

    std::string r(pszAnsi);
    delete[] pszAnsi;

    TJINFO("UTF8ToANSI %s", r.c_str());

    return r;
}
#endif
*/

bool PCZipExtract::zipExtract(std::string filePath, std::string dirPath,CallBack2 call)
{
    //TJINFO("PCZipExtract::zipExtract %s, %s", filePath.c_str(), dirPath.c_str());
    //#define SZ_OK 0
    //    
    //#define SZ_ERROR_DATA 1
    //#define SZ_ERROR_MEM 2
    //#define SZ_ERROR_CRC 3
    //#define SZ_ERROR_UNSUPPORTED 4
    //#define SZ_ERROR_PARAM 5
    //#define SZ_ERROR_INPUT_EOF 6
    //#define SZ_ERROR_OUTPUT_EOF 7
    //#define SZ_ERROR_READ 8
    //#define SZ_ERROR_WRITE 9
    //#define SZ_ERROR_PROGRESS 10
    //#define SZ_ERROR_FAIL 11
    //#define SZ_ERROR_THREAD 12
    //    
    //#define SZ_ERROR_ARCHIVE 16
    //#define SZ_ERROR_NO_ARCHIVE 17
    
    _extractCall = call;
    destoryThread();
    _extractThread = new std::thread([=](PCZipExtract* obj){
        C7z o7z;
        int ret = o7z.extract(filePath.c_str(), dirPath.c_str(), [](int curFileCount,int totalFileCount){
            int percent = 0;
            if (totalFileCount > 0) {
                percent = curFileCount * 100.0 / totalFileCount;
            }
            percent = percent >= 100 ? 99 : percent;
            PCZipExtract::getInstance()->_extractCall(0,percent);
        });
        if(ret != 0)
        {
            _extractCall(ret,0);
        }
        else
        {
            _extractCall(ret,100);

		#ifdef WIN32
			_wunlink(utf8string2wstring(filePath).c_str());
		#else
            unlink(filePath.c_str());
		#endif
        }

    },this);
    return true;
}
