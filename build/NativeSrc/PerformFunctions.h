#ifndef	PERFORM_FUNCTIONS_H_
#define	PERFORM_FUNCTIONS_H_

extern "C"
{
    #include <lua.h>

    LUA_API int luaopen_PerformFunctions (lua_State *L);
}
typedef void(* UnityLogCallback)(const char* str);

#ifdef __cplusplus
extern "C" {
#endif
LUA_API int RegisteLoggerCB(UnityLogCallback callback);
LUA_API void removeAllFunctionsInUnityMainThread();
LUA_API void performFunctionInUnityMainThread();
#ifdef __cplusplus
}
#endif

#endif