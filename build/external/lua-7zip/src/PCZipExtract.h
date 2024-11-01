//
//  PCZipExtract.hpp
//  PCHotUpdate
//
//  Created by caoxin on 2017/4/7.
//  Copyright © 2017年 caoxin. All rights reserved.
//

#ifndef PCZipExtract_hpp
#define PCZipExtract_hpp

#include <stdio.h>
#include "PCDefine.h"

#if WIN32
#include <codecvt>
using namespace std
;
#endif

NAMESPACE_BEGIN

class PCZipExtract
{
public:
    PCZipExtract();
    ~PCZipExtract();
    static PCZipExtract* getInstance();
    static void destoryInstance();
public:
    bool zipIntegrity(std::string filePath,CallBack2 call);
    bool zipExtract(std::string filePath, std::string dirPath,CallBack2 call);
    void destoryThread();

    CallBack2 _integrityTestCall;
    CallBack2 _extractCall;
private:
    std::thread* _extractThread;

#if WIN32
	inline std::wstring utf8string2wstring(const std::string& str)
	{
		static std::wstring_convert< std::codecvt_utf8<wchar_t> > strCnv;
		return strCnv.from_bytes(str);
	}
#endif  

};
NAMESPACE_END
#endif /* PCZipExtract_hpp */
