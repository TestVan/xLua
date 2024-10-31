#include <vector>
#include <mutex>
#include <functional>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include <stdarg.h>

void UnityLog(const char* str, ...);
}

#include "PerformFunctions.h"
// #include "platform/CCFileUtils.h"

static UnityLogCallback _unityLog;

static std::vector<std::function<void()>> _functionsToPerform;
static std::mutex _performMutex;

void performFunctionInLuaThread(const std::function<void ()> &function)
{
    _performMutex.lock();

    _functionsToPerform.push_back(function);

    _performMutex.unlock();
}

void removeAllFunctionsToBePerformedInCocosThread()
{
    std::unique_lock<std::mutex> lock(_performMutex);
    _functionsToPerform.clear();
}

void kcpPerformInLuaThread(const std::function<void()> &function)
{
    performFunctionInLuaThread(function);
}

void pigeonPerformInLuaThread(const std::function<void()> &function)
{
    performFunctionInLuaThread(function);
}

LUA_API int RegisteLoggerCB(UnityLogCallback callback)
{
  _unityLog = callback;
  return 0;
}

void UnityLog(const char* str, ...)
{
  if (_unityLog != NULL)
  {
    va_list args;
    va_start(args, str);
    char t[1024];
    vsnprintf(t, 1024, str, args);
    va_end(args);
    _unityLog(t);
  }
}

void performFunctionInMainThread()
{

	if (!_functionsToPerform.empty())
    {
        _performMutex.lock();
        // fixed #4123: Save the callback functions, they must be invoked after '_performMutex.unlock()', otherwise if new functions are added in callback, it will cause thread deadlock.
        auto temp = _functionsToPerform;
        _functionsToPerform.clear();

        _performMutex.unlock();
        for( const auto &function : temp )
        {
            function();
        }
        
    }
}

LUA_API void removeAllFunctionsInUnityMainThread()
{
  removeAllFunctionsToBePerformedInCocosThread();
}

LUA_API void performFunctionInUnityMainThread()
{
    performFunctionInMainThread();
}


static int l_removeAllFunctionsToBePerformedInCocosThread (lua_State *L) {
       removeAllFunctionsToBePerformedInCocosThread();
       return 0;
    }

static int l_performFunctionInMainThread (lua_State *L) {
       performFunctionInMainThread();
       return 0;
    }

static const struct luaL_Reg mylib [] = {
      {"removeAllFunctionsToBePerformedInCocosThread", l_removeAllFunctionsToBePerformedInCocosThread},
      {"performFunctionInMainThread", l_performFunctionInMainThread},
      {NULL, NULL}  /* sentinel */
    };

LUA_API int luaopen_PerformFunctions (lua_State *L) {
      luaL_openlib(L, "PerformFunctions", mylib, 0);
      return 1;
    }