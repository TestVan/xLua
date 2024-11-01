//
//  lua_app_7zip.hpp
//  demo
//
//  Created by caoxin on 2017/4/10.
//
//

#ifndef lua_app_7zip_hpp
#define lua_app_7zip_hpp

#ifdef __cplusplus
extern "C" {
#endif
#include "tolua++.h"
#ifdef __cplusplus
}
#endif

#include <functional>

extern void performFunctionInLuaThread(const std::function<void()>& function);

extern "C"
{
    #include <lua.h>

    LUA_API int register_app_7zip(lua_State* tolua_S);
}


#endif /* lua_app_7zip_hpp */
