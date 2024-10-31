	
#ifndef __CC_LUA_STACK_H_
#define __CC_LUA_STACK_H_

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "tolua++.h"

#ifdef __cplusplus
extern "C"
{
#endif
    TOLUA_API int isRef(lua_State *L, int ref);
    TOLUA_API int ref(lua_State *L, int idx);
    TOLUA_API void deref(lua_State *L, int ref);
    TOLUA_API void retainRef(lua_State *L, int ref);
    TOLUA_API void releaseRef(lua_State *L, int ref);
    TOLUA_API void pushInt(lua_State *L, int intValue);
    TOLUA_API void pushString(lua_State *L, const char* stringValue);
    TOLUA_API int executeFunction(lua_State *L, int numArgs);
    TOLUA_API int executeCall(lua_State *L, int numArgs, int numResults);

#ifdef __cplusplus
} // extern "C"
#endif

// end group
/// @}
#endif // __CC_LUA_STACK_H_
