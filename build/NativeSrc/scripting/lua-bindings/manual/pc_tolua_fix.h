#ifndef pc_tolua_fix_h__
#define pc_tolua_fix_h__

#include "tolua++.h"
#include "tolua_fix.h"


#ifdef __cplusplus
extern "C"
{
#endif
    #define TOLUA_PC_REFID_MAPPING "__tolua_pc_refid_mapping"
    
    TOLUA_API int tolua_pc_isobject(lua_State* L, int lo, int def, tolua_Error* err);
    TOLUA_API int tolua_pc_isfunction(lua_State* L, int lo, int def, tolua_Error* err);
    
    /**
     *
     */
    TOLUA_API void tolua_pc_rebaseref(lua_State* L);
    TOLUA_API int  tolua_pc_isref(lua_State* L, int ref);
    TOLUA_API int  tolua_pc_ref(lua_State* L, const char* registryName, int idx);
    TOLUA_API void tolua_pc_retainref(lua_State* L, const char* registryName, int ref);
    TOLUA_API void tolua_pc_deref(lua_State* L, const char* registryName, int ref);
    TOLUA_API void tolua_pc_releaseref(lua_State* L, const char* registryName, int ref);
	
#ifdef __cplusplus
} // extern "C"
#endif


#endif // pc_tolua_fix_h__
