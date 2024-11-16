//
//  lua_app_hotupdate_manual.cpp
//  demo
//
//  Created by caoxin on 2017/4/10.
//
//

#include "lua_app_7zip.hpp"
#include "scripting/lua-bindings/manual/pc_tolua_fix.h"
#include "scripting/lua-bindings/manual/tolua_fix.h"

#include "scripting/lua-bindings/manual/LuaStack.h"

#include "PCZipExtract.h"
#include "lua.h"

static lua_State *s_mainThread = nullptr;

using namespace gamebase;
static volatile void crash() {
    int* p = NULL;
    *p = 0;
}

static int lua_zipextract_extractfile(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (!tolua_isstring(tolua_S,1,0,&tolua_err) ||
        !tolua_isstring(tolua_S,2,0,&tolua_err) ||
        !toluafix_isfunction(tolua_S,3,"LUA_FUNCTION",0,&tolua_err)
        )
        goto tolua_lerror;
    else
#endif
    {
        std::string filePath = tolua_tostring(tolua_S,1,0);
        std::string dirPath = tolua_tostring(tolua_S,2,0);
        int handler = tolua_pc_ref(tolua_S, TOLUA_PC_REFID_MAPPING, 3);
        
        CallBack2 callback;
        
        callback = [handler](int error, int percent) {
            performFunctionInLuaThread([=]() {
                if (!tolua_pc_isref(s_mainThread, handler)) {
                    return;
                }
                
                tolua_pc_deref(s_mainThread, TOLUA_PC_REFID_MAPPING, handler);
                tolua_pushnumber(s_mainThread, error);
                tolua_pushnumber(s_mainThread, percent);
                executeCall(s_mainThread, 2, 1);
                
                if (!lua_isnil(s_mainThread, 1) && lua_toboolean(s_mainThread, 1) != 0) {
                    tolua_pc_releaseref(s_mainThread, TOLUA_PC_REFID_MAPPING, handler);
                    PCZipExtract::getInstance()->destoryInstance();
                }
                
                lua_pop(s_mainThread, 1);
            });
        };
        bool ret = PCZipExtract::getInstance()->zipExtract(filePath, dirPath, callback);
        tolua_pushboolean(tolua_S, ret);
        return 1;
    }
    return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_zipextract_extractfile'.",&tolua_err);
    return 0;
#endif
}


LUA_API int register_app_7zip (lua_State *l) 
{
    luaL_Reg reg[] = {
        { "extractFile", lua_zipextract_extractfile },
        { NULL, NULL }
    };

#if LUA_VERSION_NUM > 501 && !defined(LUA_COMPAT_MODULE)
    lua_newtable(l);
    luaL_setfuncs(l, reg, 0);
    lua_setglobal(l, "c7zip");
#else
    luaL_openlib(l, "c7zip", reg, 0);
#endif

    s_mainThread = l;
    return 1;
}
