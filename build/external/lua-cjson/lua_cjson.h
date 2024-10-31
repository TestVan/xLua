
#ifndef __LUA_CJSON_H_
#define __LUA_CJSON_H_

#include "lua.h"

#define USE_INTERNAL_FPCONV
#define ENABLE_CJSON_GLOBAL 1


	LUA_API  int luaopen_cjson(lua_State *l);
	LUA_API  int luaopen_cjson_safe(lua_State *l);




	



#endif // __LUA_CJSON_H_
