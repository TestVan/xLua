
#ifndef __LUABAISCCONVERSIONS_H__
#define __LUABAISCCONVERSIONS_H__

#include <unordered_map>
#include <string>

extern "C" {
#include "luaconf.h"
#include "lua.h"
#include "tolua++.h"
}
#include "scripting/lua-bindings/manual/tolua_fix.h"
#include "scripting/lua-bindings/manual/Lua-BindingsExport.h"

#include <map>
#include <typeinfo>
#include <vector>
#include <string>

extern std::unordered_map<std::string, std::string>  g_luaType;
extern std::unordered_map<std::string, std::string>  g_typeCast;


#define LUA_PRECONDITION( condition, ...) if( ! (condition) ) {														\
cocos2d::log("lua: ERROR: File %s: Line: %d, Function: %s", __FILE__, __LINE__, __FUNCTION__ );                                                         \
cocos2d::log(__VA_ARGS__);                                                  \
}                                                                      \

/**
 * @addtogroup lua
 * @{
 */

/**
 * If the typename of userdata at the given acceptable index of stack is equal to type it return true, otherwise return false.
 * If def != 0, lo could greater than the top index of stack, return value is true.
 * If the value of the given index is nil, return value also is true.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param type the typename used to judge.
 * @param def whether has default value.
 * @return Return true if the typename of userdata at the given acceptable index of stack is equal to type, otherwise return false.
 */
extern bool luaval_is_usertype(lua_State* L,int lo,const char* type, int def);
// to native

/**
 * @name luaval_to_native
 * The following function are all used to convert the Lua values at the given acceptable index to the corresponding c++ values.
 * If the Lua values can be converted the return value is true, otherwise return false.
 * If it happens error during the conversion process, it outputs the error msg in the console to provide information about the name of calling function, the typename of value at the given acceptable index, and so on.
 * @{
 **/

/**
 * Get a unsigned long value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a number or a string convertible to a number it returns true, otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a number, otherwise return false.
 */
extern bool luaval_to_ulong(lua_State* L,int lo, unsigned long* outValue, const char* funcName="");

/**
 * Get a unsigned short value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a number or a string convertible to a number it returns true, otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the unsigned short value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a number, otherwise return false.
 */
extern bool luaval_to_ushort(lua_State* L, int lo, unsigned short* outValue, const char* funcName = "");

/**
 * Get a int value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a number or a string convertible to a number it returns true, otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the int value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a number, otherwise return false.
 */
extern bool luaval_to_int32(lua_State* L,int lo,int* outValue, const char* funcName = "");

/**
 * Get a unsigned int value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a number or a string convertible to a number it returns true, otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the unsigned int value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a number, otherwise return false.
 */
extern bool luaval_to_uint32(lua_State* L, int lo, unsigned int* outValue, const char* funcName = "");

/**
 * Get a uint16_t value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a number or a string convertible to a number it returns true, otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the uint16_t value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a number, otherwise return false.
 */
extern bool luaval_to_uint16(lua_State* L,int lo,uint16_t* outValue, const char* funcName = "");

/**
 * Get a boolean value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack has type boolean it returns true, otherwise returns false.
 * Any Lua value is different from false and nil, the value of conversion is true, otherwise the value is false.
 * If the lo is non-valid index, the value of conversion also is false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the boolean value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a number, otherwise return false.
 */
extern bool luaval_to_boolean(lua_State* L,int lo,bool* outValue, const char* funcName = "");

/**
 * Get a double value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a number or a string convertible to a number it returns true, otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the double value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a number, otherwise return false.
 */
extern bool luaval_to_number(lua_State* L,int lo,double* outValue, const char* funcName = "");

/**
 * Get a long long value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a number or a string convertible to a number it returns true, otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the long long value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a number, otherwise return false.
 */
extern bool luaval_to_long_long(lua_State* L,int lo,long long* outValue, const char* funcName = "");

/**
 * Get a std::string value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a string or a number convertible to a string it returns true, otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store std::string value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a string or a number convertible to a string, otherwise return false.
 */
extern bool luaval_to_std_string(lua_State* L, int lo, std::string* outValue, const char* funcName = "");

/**
 * Get a long value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a number or a string convertible to a number it returns true, otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the long value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a number, otherwise return false.
 */
extern bool luaval_to_long(lua_State* L,int lo, long* outValue, const char* funcName = "");

/**
 * Get a ssize_t value from the given acceptable index of stack.
 * If the value at the given acceptable index of stack is a number or a string convertible to a number it returns true, otherwise returns false.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param outValue the pointer to store the ssize_t value converted from the Lua value.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a number or a string convertible to a number, otherwise return false.
 */
extern bool luaval_to_ssize(lua_State* L,int lo, ssize_t* outValue, const char* funcName = "");

/**
 * Get a pointer points to a std::vector<std::string> from a Lua array table in the stack.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param ret a pointer points to a std::vector<std::string>.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
extern bool luaval_to_std_vector_string(lua_State* L, int lo, std::vector<std::string>* ret, const char* funcName = "");

/**
 * Get a pointer points to a std::vector<std::int> from a Lua array table in the stack.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param ret a pointer points to a std::vector<int>.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
extern bool luaval_to_std_vector_int(lua_State* L, int lo, std::vector<int>* ret, const char* funcName = "");

/**
 * Get a Type T object from the given acceptable index of stack.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param type a string pointer points to the type name.
 * @param ret the pointer points to a Type T object.
 * @return Return true if the type of Lua value at the index is a Lua userdata, otherwise return false.
 */
template <class T>
bool luaval_to_object(lua_State* L, int lo, const char* type, T** ret, const char* funcName = "")
{
    if(nullptr == L || lua_gettop(L) < lo)
        return false;

    if (!luaval_is_usertype(L, lo, type, 0))
        return false;

    *ret = static_cast<T*>(tolua_tousertype(L, lo, 0));

    #if !UNITY
    if (nullptr == *ret)
        CCLOG("Warning: %s argument %d is invalid native object(nullptr)", funcName, lo);
   	#endif

    return true;
}

/**
 * Get a pointer points to a std::vector<float> from a Lua array table in the stack.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param ret a pointer points to a std::vector<float>.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
extern bool luaval_to_std_vector_float(lua_State* L, int lo, std::vector<float>* ret, const char* funcName = "");

/**
 * Get a pointer points to a std::vector<unsigned shortt> from a Lua array table in the stack.
 *
 * @param L the current lua_State.
 * @param lo the given acceptable index of stack.
 * @param ret a pointer points to a std::vector<unsigned short>.
 * @param funcName the name of calling function, it is used for error output in the debug model.
 * @return Return true if the value at the given acceptable index of stack is a table, otherwise return false.
 */
extern bool luaval_to_std_vector_ushort(lua_State* L, int lo, std::vector<unsigned short>* ret, const char* funcName = "");

extern bool luaval_to_std_map_string_string(lua_State* L, int lo, std::map<std::string, std::string>* ret, const char* funcName);

/**
 * Get the real typename for the specified typename.
 * Because all override functions wouldn't be bound,so we must use `typeid` to get the real class name.
 *
 * @param ret the pointer points to a type T object.
 * @param type the string pointer points to specified typename.
 * @return return the pointer points to the real typename, or nullptr.
 */
template <class T>
const char* getLuaTypeName(T* ret,const char* type)
{
    if (nullptr != ret)
    {
        std::string hashName = typeid(*ret).name();
        auto iter =  g_luaType.find(hashName);
        if(g_luaType.end() != iter)
        {
            return iter->second.c_str();
        }
        else
        {
            return type;
        }
    }

    return nullptr;
}
/**
 * Push the native object by userdata format into the Lua stack by typename.
 *
 * @param L the current lua_State.
 * @param type the string pointer points to the typename.
 * @param ret the native object pointer.
 */
template <class T>
void object_to_luaval(lua_State* L,const char* type, T* ret)
{
    if(nullptr != ret)
    {
		tolua_pushusertype(L,(void*)ret,getLuaTypeName(ret, type));
    }
    else
    {
        lua_pushnil(L);
    }
}

/**
 * Push a Lua dict table converted from a std::map<std::string, std::string> into the Lua stack.
 *
 * @param L the current lua_State.
 * @param inValue a std::map<std::string, std::string> value.
 */
extern void std_map_string_string_to_luaval(lua_State* L, const std::map<std::string, std::string>& inValue);


#endif //__LUABAISCCONVERSIONS_H__
