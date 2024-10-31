#include "lua-bindings/manual/LuaStack.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lua-bindings/manual/pc_tolua_fix.h"
#include "tolua++.h"

// begin from cocos
TOLUA_API int isRef(lua_State *L, int ref)
{
    return tolua_pc_isref(L, ref)!=0;
}
TOLUA_API int ref(lua_State *L, int idx)
{
    return tolua_pc_ref(L, TOLUA_PC_REFID_MAPPING, idx);
}
TOLUA_API void deref(lua_State *L, int ref)
{
    tolua_pc_deref(L, TOLUA_PC_REFID_MAPPING, ref);
}
TOLUA_API void retainRef(lua_State *L, int ref)
{
    tolua_pc_retainref(L, TOLUA_PC_REFID_MAPPING, ref);
}
TOLUA_API void releaseRef(lua_State *L, int ref)
{
    tolua_pc_releaseref(L, TOLUA_PC_REFID_MAPPING, ref);
}
TOLUA_API void pushString(lua_State *L, const char* stringValue)
{
    lua_pushstring(L, stringValue);
}
TOLUA_API void pushInt(lua_State *L, int intValue)
{
    lua_pushinteger(L, intValue);
}
TOLUA_API int executeFunction(lua_State *L, int numArgs)
{
    int functionIndex = -(numArgs + 1);
    if (!lua_isfunction(L, functionIndex))
    {
        luaL_error(L, "value at stack [%d] is not function", functionIndex);
        lua_pop(L, numArgs + 1); // remove function and arguments
        return 0;
    }

    int traceback = 0;
    lua_getglobal(L, "__G__TRACKBACK__");                         /* L: ... func arg1 arg2 ... G */
    if (!lua_isfunction(L, -1))
    {
        lua_pop(L, 1);                                            /* L: ... func arg1 arg2 ... */
    }
    else
    {
        lua_insert(L, functionIndex - 1);                         /* L: ... G func arg1 arg2 ... */
        traceback = functionIndex - 1;
    }

    int error = 0;
    // ++_callFromLua;
    error = lua_pcall(L, numArgs, 1, traceback);                  /* L: ... [G] ret */
    // --_callFromLua;
    if (error)
    {
        if (traceback == 0)
        {
            luaL_error(L, "[LUA ERROR] %s", lua_tostring(L, - 1));        /* L: ... error */
            lua_pop(L, 1); // remove error message from stack
        }
        else                                                            /* L: ... G error */
        {
            lua_pop(L, 2); // remove __G__TRACKBACK__ and error message from stack
        }
        return 0;
    }

    // get return value
    int ret = 0;
    if (lua_isnumber(L, -1))
    {
        ret = (int)lua_tointeger(L, -1);
    }
    else if (lua_isboolean(L, -1))
    {
        ret = (int)lua_toboolean(L, -1);
    }
    // remove return value from stack
    lua_pop(L, 1);                                                /* L: ... [G] */

    if (traceback)
    {
        lua_pop(L, 1); // remove __G__TRACKBACK__ from stack      /* L: ... */
    }

    return ret;
}
TOLUA_API int executeCall(lua_State *L, int numArgs, int numResults)
{
    int functionIndex = -(numArgs + 1);
    if (!lua_isfunction(L, functionIndex))
    {
        luaL_error(L, "value at stack [%d] is not function", functionIndex);
        lua_pop(L, numArgs + 1); // remove function and arguments
        return -1;
    }
    
    int traceback = 0;
    lua_getglobal(L, "__G__TRACKBACK__");                         /* L: ... func arg1 arg2 ... G */
    if (!lua_isfunction(L, -1))
    {
        lua_pop(L, 1);                                            /* L: ... func arg1 arg2 ... */
    }
    else
    {
        lua_insert(L, functionIndex - 1);                         /* L: ... G func arg1 arg2 ... */
        traceback = functionIndex - 1;
    }
    
    int error = 0;
    error = lua_pcall(L, numArgs, numResults, traceback);                  /* L: ... [G] {rets} */
    if (error)
    {
        if (traceback == 0)
        {
            luaL_error(L, "[LUA ERROR] %s", lua_tostring(L, - 1));        /* L: ... error */
            lua_pop(L, 1); // remove error message from stack
        }
        else                                                            /* L: ... G error */
        {
            lua_pop(L, 2); // remove __G__TRACKBACK__ and error message from stack
        }
        return -2;
    }
    
    if (traceback!=0) {
        // remove __G__TRACKBACK__ from stack
        lua_remove(L, -numResults-1); /* L: ... {rets} */
    }
    
    return numResults;
}
//end from cocos