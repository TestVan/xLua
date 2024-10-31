#include "pc_tolua_fix.h"
#include <stdlib.h>

#define DEFAULT_REFTABLE_SIZE 4096

TOLUA_API int tolua_pc_isobject(lua_State* L, int lo, int def, tolua_Error* err)
{
    if (def && lua_gettop(L)<abs(lo))
        return 1;
    if (lua_istable(L,lo) || lua_isuserdata(L,lo))
        return 1;
    err->index = lo;
    err->array = 0;
    err->type = "object(table/userdata)";
    return 0;
}

TOLUA_API int tolua_pc_isfunction (lua_State* L, int lo, int def, tolua_Error* err)
{
    if (def && lua_gettop(L)<abs(lo))
        return 1;
    if (lua_isfunction(L,lo))
        return 1;
    err->index = lo;
    err->array = 0;
    err->type = "function";
    return 0;
}

/* convert a stack index to positive */
#define abs_index(L, i)		((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : lua_gettop(L) + (i) + 1)

static void get_or_create_table(lua_State* L, int father, const char* name, int narr, int nrec)
{
    int t = abs_index(L, father);
    lua_getfield(L, t, name); /* get t[name] */
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        
        /* t[name] = {} */
//        lua_newtable(L);
        lua_createtable(L, narr, nrec);
        lua_setfield(L, t, name);
        
        lua_getfield(L, t, name); /* get t[name] */
    }
}

static int __ref_base = 0;
static int __ref_max = -1;

#if defined(WIN32)
#define inline __inline
#endif

static inline int ref_to_logical_ref(int ref) {
    int lref = ref==LUA_REFNIL ? LUA_REFNIL : (__ref_base + ref);
    if (lref > __ref_max) { __ref_max = lref; }
    return lref;
}

static inline int logical_ref_to_ref(int lref) {
    return lref==LUA_REFNIL ? LUA_REFNIL : (lref - __ref_base);
}

static inline int is_logical_ref(int lref) {
    return (lref>=__ref_base && lref<=__ref_max) ? 1 : 0;
}

TOLUA_API void tolua_pc_rebaseref(lua_State* L)
{
    __ref_base = __ref_max + 1;
}

TOLUA_API int  tolua_pc_isref(lua_State* L, int ref)
{
    if (ref==LUA_REFNIL || is_logical_ref(ref)) {
        return 1;
    }
    return 0;
}

TOLUA_API int tolua_pc_ref(lua_State* L, const char* registryName, int idx)
{
    int ref;
    lua_pushvalue(L, idx);
    
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);  /* remove from stack */
        return LUA_REFNIL;  /* `nil' has a unique fixed reference */
    }
    
    /* pushed a table on the top of stack */
    get_or_create_table(L, LUA_REGISTRYINDEX, registryName, DEFAULT_REFTABLE_SIZE, DEFAULT_REFTABLE_SIZE);
    
//    if (!lua_istable(L, -2) && !lua_isuserdata(L, -2) && !lua_isfunction(L, -2)) {
//        lua_pushvalue(L, -2);
//        ref = ref_to_logical_ref(luaL_ref(L, -2));
//        lua_pop(L, 2);
//        return ref;
//    }
    
    get_or_create_table(L, -1, "__referenced", 0, DEFAULT_REFTABLE_SIZE);
    lua_pushvalue(L, -3); /* copy the value to the top of stack */
    lua_rawget(L, -2);  /* info (at the top of stack) = t["__referenced"] */
    if (lua_istable(L, -1)) {
        /* ref = info[refid] */
        lua_getfield(L, -1, "refid");
        ref = (int)lua_tointeger(L, -1);
        lua_pop(L, 1);
        
        /* info[refcnt] = info[refcnt]+1 */
        lua_getfield(L, -1, "refcnt");
        lua_Integer refcnt = lua_tointeger(L, -1);
        lua_pop(L, 1);
        lua_pushinteger(L, refcnt+1);
        lua_setfield(L, -2, "refcnt");
        
        /* stack: value, registry, __referenced, info */
        lua_pop(L, 4);
        
        return ref;
    }
    
    lua_pushvalue(L, -4); /* copy the value to the top of stack */
    ref = ref_to_logical_ref(luaL_ref(L, -4));
    
    lua_pop(L, 1);
    lua_pushvalue(L, -3);
//    lua_newtable(L);
    lua_createtable(L, 0, 2);
    
    /* t["refid"] = ref */
    lua_pushinteger(L, ref);
    lua_setfield(L, -2, "refid");
    /* t["refcnt"] = 1 */
    lua_pushinteger(L, 1);
    lua_setfield(L, -2, "refcnt");
    
    /* __referenced[value] = t */
    lua_rawset(L, -3);
    
    /* stack: value, registry, __referenced */
    lua_pop(L, 3);
    
    return ref;
}

TOLUA_API void tolua_pc_retainref(lua_State* L, const char* registryName, int ref)
{
    if (ref==LUA_REFNIL || !is_logical_ref(ref)) {
        return;
    }
    
    /* registry = lua_registry[registryName] */
    lua_getfield(L, LUA_REGISTRYINDEX, registryName);
    if (lua_isnoneornil(L, -1)) {
        lua_pop(L, 1);
        return;
    }
    
    lua_rawgeti(L, -1, logical_ref_to_ref(ref));
//    if (!lua_istable(L, -1) && !lua_isuserdata(L, -1) && !lua_isfunction(L, -1)) {
//        lua_pop(L, 2);
//        return;
//    }
    
    lua_getfield(L, -2, "__referenced");
    lua_pushvalue(L, -2); /* copy the value to the top of stack */
    lua_rawget(L, -2);  /* info (at the top of stack) = t["__referenced"] */
    /* assert(lua_istable(L, -1)) */
    
    /* refcnt = refcnt+1 */
    lua_getfield(L, -1, "refcnt");
    lua_Integer refcnt = lua_tointeger(L, -1);
    lua_pop(L, 1);
    refcnt++;
    lua_pushinteger(L, refcnt);
    lua_setfield(L, -2, "refcnt");
    
    /* stack: registry, value, __referenced, info */
    lua_pop(L, 4);
    return;
}

TOLUA_API void tolua_pc_deref(lua_State* L, const char* registryName, int ref)
{
    if (ref==LUA_REFNIL || !is_logical_ref(ref)) {
        lua_pushnil(L);
        return;
    }
    
    /* registry = lua_registry[registryName] */
    lua_getfield(L, LUA_REGISTRYINDEX, registryName);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_pushnil(L);
        return;
    }
    
    lua_rawgeti(L, -1, logical_ref_to_ref(ref));
    lua_replace(L, -2);
    return;
}

TOLUA_API void tolua_pc_releaseref(lua_State* L, const char* registryName, int ref)
{
    if (ref==LUA_REFNIL || !is_logical_ref(ref)) {
        return;
    }
    
    /* registry = lua_registry[registryName] */
    lua_getfield(L, LUA_REGISTRYINDEX, registryName);
    if (lua_isnoneornil(L, -1)) {
        lua_pop(L, 1);
        return;
    }
    
    int rawref = logical_ref_to_ref(ref);
    lua_rawgeti(L, -1, rawref);
//    if (!lua_istable(L, -1) && !lua_isuserdata(L, -1) && !lua_isfunction(L, -1)) {
//        luaL_unref(L, -2, rawref);
//        lua_pop(L, 2);
//        return;
//    }
    
    lua_getfield(L, -2, "__referenced");
    lua_pushvalue(L, -2); /* copy the value to the top of stack */
    lua_rawget(L, -2);  /* info (at the top of stack) = t["__referenced"] */
    /* assert(lua_istable(L, -1)) */
    
    lua_getfield(L, -1, "refcnt");
    lua_Integer refcnt = lua_tointeger(L, -1);
    lua_pop(L, 1);
    refcnt--;
    if (refcnt==0) {
        lua_pop(L, 1);
        /* __referenced[value] = nil */
        lua_pushvalue(L, -2);
        lua_pushnil(L);
        lua_rawset(L, -3);
        
        luaL_unref(L, -3, rawref);
        lua_pop(L, 3);
        return;
    }
    
    lua_pushinteger(L, refcnt);
    lua_setfield(L, -2, "refcnt");
    
    /* stack: registry, value, __referenced, info */
    lua_pop(L, 4);
    return;
}
