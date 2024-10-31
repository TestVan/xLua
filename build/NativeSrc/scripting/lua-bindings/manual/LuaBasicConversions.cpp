/****************************************************************************
 Copyright (c) 2013-2017 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "scripting/lua-bindings/manual/LuaBasicConversions.h"
#include "scripting/lua-bindings/manual/tolua_fix.h"

/* convert a stack index to positive */
#define abs_index(L, i)		((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : lua_gettop(L) + (i) + 1)

#include <cmath>
#include <cstring>
#include <limits>

#define CCASSERT(...)   

std::unordered_map<std::string, std::string>  g_luaType;
std::unordered_map<std::string, std::string>  g_typeCast;

#if LUA_VERSION_NUM >= 503

#define lua_objlen(L, i)  lua_rawlen((L), (i))

int push_table_instance(lua_State* L, int lo) {

    if (lua_istable(L, lo)) {

        lua_pushstring(L, ".c_instance");
        lua_gettable(L, lo);
        if (lua_isuserdata(L, -1)) {

            lua_replace(L, lo);
            return 1;
        } else {

            lua_pop(L, 1);
            return 0;
        };
    } else {
        return 0;
    };

    return 0;
};
/* the equivalent of lua_is* for usertype */
int lua_isusertype (lua_State* L, int lo, const char* type)
{
    if (!lua_isuserdata(L,lo)) {
        if (!push_table_instance(L, lo)) {
            return 0;
        };
    };
    {
        /* check if it is of the same type */
        int r;
        const char *tn;
        if (lua_getmetatable(L,lo))        /* if metatable? */
        {
            lua_rawget(L,LUA_REGISTRYINDEX);  /* get registry[mt] */
            tn = lua_tostring(L,-1);
            r = tn && (strcmp(tn,type) == 0);
            lua_pop(L, 1);
            if (r)
                return 1;
            else
            {
                /* check if it is a specialized class */
                lua_pushstring(L,"tolua_super");
                lua_rawget(L,LUA_REGISTRYINDEX); /* get super */
                lua_getmetatable(L,lo);
                lua_rawget(L,-2);                /* get super[mt] */
                if (lua_istable(L,-1))
                {
                    int b;
                    lua_pushstring(L,type);
                    lua_rawget(L,-2);                /* get super[mt][type] */
                    b = lua_toboolean(L,-1);
                    lua_pop(L,3);
                    if (b)
                        return 1;
                }
            }
        }
    }
    return 0;
}
#else
#ifdef __cplusplus
extern "C" {
#endif
extern int lua_isusertype (lua_State* L, int lo, const char* type);
#ifdef __cplusplus
}
#endif
#endif

bool luaval_is_usertype(lua_State* L,int lo,const char* type, int def)
{
    if (def && lua_gettop(L)<std::abs(lo))
        return true;

    if (lua_isnil(L,lo) || lua_isusertype(L,lo,type))
        return true;

    return false;
}

bool luaval_to_ushort(lua_State* L, int lo, unsigned short* outValue, const char* funcName)
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L,lo,0,&tolua_err))
    {

        ok = false;
    }

    if (ok)
    {
        *outValue = (unsigned short)tolua_tonumber(L, lo, 0);
    }

    return ok;
}

bool luaval_to_int32(lua_State* L,int lo,int* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L,lo,0,&tolua_err))
    {

        ok = false;
    }

    if (ok)
    {
        /**
         When we want to convert the number value from the Lua to int, we would call lua_tonumber to implement.It would
         experience two phase conversion: int -> double, double->int.But,for the 0x80000000 which the min value of int, the
         int cast may return an undefined result,like 0x7fffffff.So we must use the (int)(unsigned int)lua_tonumber() to get
         predictable results for 0x80000000.In this place,we didn't use lua_tointeger, because it may produce different results
         depending on the compiler,e.g:for iPhone4s,it also get wrong value for 0x80000000.
         */
        unsigned int estimateValue = (unsigned int)lua_tonumber(L, lo);
        if (estimateValue == std::numeric_limits<int>::min())
        {
            *outValue = (int)estimateValue;
        }
        else
        {
            *outValue = (int)lua_tonumber(L, lo);
        }
    }

    return ok;
}

bool luaval_to_uint32(lua_State* L, int lo, unsigned int* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L,lo,0,&tolua_err))
    {

        ok = false;
    }

    if (ok)
    {
        *outValue = (unsigned int)tolua_tonumber(L, lo, 0);
    }

    return ok;
}

bool luaval_to_uint16(lua_State* L,int lo,uint16_t* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L,lo,0,&tolua_err))
    {

        ok = false;
    }

    if (ok)
    {
        *outValue = (unsigned char)tolua_tonumber(L, lo, 0);
    }

    return ok;
}

bool luaval_to_boolean(lua_State* L,int lo,bool* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isboolean(L,lo,0,&tolua_err))
    {

        ok = false;
    }

    if (ok)
    {
        *outValue = (bool)tolua_toboolean(L, lo, 0);
    }

    return ok;
}

bool luaval_to_number(lua_State* L,int lo,double* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L,lo,0,&tolua_err))
    {

        ok = false;
    }

    if (ok)
    {
        *outValue = tolua_tonumber(L, lo, 0);
    }

    return ok;
}

bool luaval_to_long_long(lua_State* L,int lo,long long* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L,lo,0,&tolua_err))
    {

        ok = false;
    }

    if (ok)
    {
        *outValue = (long long)tolua_tonumber(L, lo, 0);
    }

    return ok;
}

bool luaval_to_std_string(lua_State* L, int lo, std::string* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_iscppstring(L,lo,0,&tolua_err))
    {

        ok = false;
    }

    if (ok)
    {
        size_t size;
        auto rawString = lua_tolstring(L,lo,&size);
        *outValue = std::string(rawString, size);
    }

    return ok;
}

bool luaval_to_ssize(lua_State* L,int lo, ssize_t* outValue, const char* funcName)
{
    return luaval_to_long(L, lo, reinterpret_cast<long*>(outValue));
}

bool luaval_to_long(lua_State* L,int lo, long* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L,lo,0,&tolua_err))
    {

        ok = false;
    }

    if (ok)
    {
        *outValue = (long)tolua_tonumber(L, lo, 0);
    }

    return ok;
}

bool luaval_to_ulong(lua_State* L,int lo, unsigned long* outValue, const char* funcName)
{
    if (NULL == L || NULL == outValue)
        return false;

    bool ok = true;

    tolua_Error tolua_err;
    if (!tolua_isnumber(L,lo,0,&tolua_err))
    {

        ok = false;
    }

    if (ok)
    {
        *outValue = (unsigned long)tolua_tonumber(L, lo, 0);
    }

    return ok;
}

bool luaval_to_std_vector_string(lua_State* L, int lo, std::vector<std::string>* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {

        ok = false;
    }

    if (ok)
    {
        size_t len = lua_objlen(L, lo);
        std::string value = "";
        for (size_t i = 0; i < len; i++)
        {
            lua_pushnumber(L, i + 1);
            lua_gettable(L,lo);
            if(lua_isstring(L, -1))
            {
                ok = luaval_to_std_string(L, -1, &value);
                if(ok)
                    ret->push_back(value);
            }
            else
            {
                CCASSERT(false, "string type is needed");
            }

            lua_pop(L, 1);
        }
    }

    return ok;
}

bool luaval_to_std_vector_int(lua_State* L, int lo, std::vector<int>* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {

        ok = false;
    }

    if (ok)
    {
        size_t len = lua_objlen(L, lo);
        for (size_t i = 0; i < len; i++)
        {
            lua_pushnumber(L, i + 1);
            lua_gettable(L,lo);
            if(lua_isnumber(L, -1))
            {
                ret->push_back((int)tolua_tonumber(L, -1, 0));
            }
            else
            {
                CCASSERT(false, "int type is needed");
            }

            lua_pop(L, 1);
        }
    }

    return ok;
}

bool luaval_to_std_vector_float(lua_State* L, int lo, std::vector<float>* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;

    if (!tolua_istable(L, lo, 0, &tolua_err))
    {

        ok = false;
    }

    if (ok)
    {
        size_t len = lua_objlen(L, lo);
        for (size_t i = 0; i < len; i++)
        {
            lua_pushnumber(L, i + 1);
            lua_gettable(L,lo);
            if(lua_isnumber(L, -1))
            {
                ret->push_back((float)tolua_tonumber(L, -1, 0));
            }
            else
            {
                CCASSERT(false, "float type is needed");
            }

            lua_pop(L, 1);
        }
    }

    return ok;
}


bool luaval_to_std_vector_ushort(lua_State* L, int lo, std::vector<unsigned short>* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;

    if (!tolua_istable(L, lo, 0, &tolua_err))
    {

        ok = false;
    }

    if (ok)
    {
        size_t len = lua_objlen(L, lo);
        for (size_t i = 0; i < len; i++)
        {
            lua_pushnumber(L, i + 1);
            lua_gettable(L,lo);
            if(lua_isnumber(L, -1))
            {
                ret->push_back((unsigned short)tolua_tonumber(L, -1, 0));
            }
            else
            {
                CCASSERT(false, "unsigned short type is needed");
            }

            lua_pop(L, 1);
        }
    }

    return ok;
}

void ccvector_std_string_to_luaval(lua_State* L, const std::vector<std::string>& inValue)
{
    if (nullptr == L)
        return;

    lua_newtable(L);

    int index = 1;

    for (const std::string& value : inValue)
    {
        lua_pushnumber(L, (lua_Number)index);
        lua_pushstring(L, value.c_str());
        lua_rawset(L, -3);
        ++index;
    }
}

void ccvector_int_to_luaval(lua_State* L, const std::vector<int>& inValue)
{
    if (nullptr == L)
        return;

    lua_newtable(L);

    int index = 1;
    for (const int value : inValue)
    {
        lua_pushnumber(L, (lua_Number)index);
        lua_pushnumber(L, (lua_Number)value);
        lua_rawset(L, -3);
        ++index;
    }
}

void ccvector_float_to_luaval(lua_State* L, const std::vector<float>& inValue)
{
    if (nullptr == L)
        return;

    lua_newtable(L);

    int index = 1;
    for (const float value : inValue)
    {
        lua_pushnumber(L, (lua_Number)index);
        lua_pushnumber(L, (lua_Number)value);
        lua_rawset(L, -3);
        ++index;
    }
}

void ccvector_ushort_to_luaval(lua_State* L, const std::vector<unsigned short>& inValue)
{
    if (nullptr == L)
        return;

    lua_newtable(L);

    int index = 1;
    for (const unsigned short value : inValue)
    {
        lua_pushnumber(L, (lua_Number)index);
        lua_pushnumber(L, (lua_Number)value);
        lua_rawset(L, -3);
        ++index;
    }
}

void std_map_string_string_to_luaval(lua_State* L, const std::map<std::string, std::string>& inValue)
{
    if (nullptr == L)
        return;

    lua_newtable(L);

    for (auto iter = inValue.begin(); iter != inValue.end(); ++iter)
    {
        lua_pushstring(L, iter->first.c_str());
        lua_pushstring(L, iter->second.c_str());
        lua_rawset(L, -3);
    }
}

bool luaval_to_std_map_string_string(lua_State* L, int lo, std::map<std::string, std::string>* ret, const char* funcName)
{
    if (nullptr == L || nullptr == ret || lua_gettop(L) < lo)
        return false;

    tolua_Error tolua_err;
    bool ok = true;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {

        ok = false;
    }

    if (!ok)
        return ok;

    lua_pushnil(L);
    std::string key;
    std::string value;
    while (lua_next(L, lo) != 0)
    {
        if (lua_isstring(L, -2) && lua_isstring(L, -1))
        {
            if (luaval_to_std_string(L, -2, &key) && luaval_to_std_string(L, -1, &value))
            {
                (*ret)[key] = value;
            }
        }
        else
        {
            CCASSERT(false, "string type is needed");
        }

        lua_pop(L, 1);
    }

    return ok;
}
