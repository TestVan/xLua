#include "scripting/lua-bindings/manual/LuaBasicConversions.h"
#include "scripting/lua-bindings/manual/tolua_fix.h"

#if USE_PIGEON
#include "lua-bindings/manual/lua_pigeon.h"
#include "lua-bindings/manual/lua_kcp.hpp"
#endif
#if USE_HOTUPDATE
#include "lua-bindings/manual/lua_app_hotupdate_manual.hpp"
#include "pchotupdate/PCHotUpdate.h"
#endif
#if USE_DBMGR
#include "lua-bindings/auto/lua_pcdbmanager_v2_auto.hpp"
#include "pcdbmanager/PCDBManager_v2.h"
#include "lua-bindings/auto/lua_sqlite3manager_auto.hpp"
#include "pcdbmanager/Sqlite3Manager.h"
#endif

#if USE_HTTPREUEST
#include "lua-bindings/lua_pchttprequest.h"
#include "pchttprequest/CommunicationManager.h"
#endif

#include "base/base64.h"

#if USE_FileUtils
#include "FileUtils.h"
#endif
#include "PerformFunctions.h"
#include "tjConfig.h"
#include <cassert>
#include <string>
#if USE_DECRYPTLUA
#include "decryptlua/decryptlua.h"
#endif
#if USE_SNAPPY
#include "snappy/include/snappy.h"
#endif

#if USE_MATHEX
#include "Lua_mathEx.h"
#endif
enum compressType
{
	NONE = 0,
	SNAPPY = 1,
	ZIP = 2,
};


extern "C"
{
#if USE_CJSON
#include "lua-cjson/lua_cjson.h"
#endif
#include <lua.h>

    void UnityLua_open(lua_State *L);
#if USE_DBMGR
    // PCDBManager API
    LUA_API void PCDBNativeSetEncryptKey(const char *encryptKey);
    LUA_API void PCDBNativeSetReadonlyPath(const char *path);
    LUA_API void PCDBNativeSetWritablePath(const char *path);
    LUA_API void PCDBNativeSetDebugPathAndTarget(const char *path, const char *key, const char *value);
    LUA_API void PCDBNativeInit();
    // LUA_API void PCDBNativeResetVersionNote();
    LUA_API bool PCDBNativeIsFirstStart();
    LUA_API bool PCDBNativeIsCoverInstall();
    LUA_API void PCDBNativeSearchURI(const char *reskey, char *buffer, int *bufferSize);
    LUA_API bool PCDBNativeIsDevMode();
    LUA_API void PCDBNativeGetGameConf(const char *key, char *buffer, int *bufferSize);
    LUA_API bool PCDBNativeCurrEncryptCanOpen(const char *dbName);
#endif

#if USE_DECRYPTLUA
    // PCDBManager API
	LUA_API void NativeInitDecryptLua(const char *encryptKey, const char *sign);
	LUA_API int NativeDecryptLua(char *buff, int size,  char* decrypt, int compressType);
#endif

#if defined(ANDROID)
    LUA_API void NativeSetJavaAssetManager(void* assetManager);
    LUA_API bool NativeFileExistInAssets(const char* fileName);
    LUA_API int NativeGetFileSize(const char* path);
    LUA_API void NativeGetFileContent(const char* path, char* buffer);
#endif

#if USE_PROTOBUF
    LUALIB_API int luaopen_pb(lua_State *L);
#endif

    LUA_API void NativeInitAES(const char *cfg);

    LUA_API void NativeExitGame();

    LUA_API void NativePerformFunctionInMainThread();

#if USE_DBMGR
    // 除了PCDB 还有别的地方在使用Sqlite
    LUA_API void NativeInitSqliteManager();
    LUA_API void NativeSetSqliteEncryptKey(const char* dbPathName, const char* encryptKey);
    LUA_API void NativeExcuteSqlite3(const char* sql, const char* dbName, char* buffer, int* bufferSize);
    LUA_API int Native_Sqlite3_prepare(const char* dbName, const char* zSql);
    LUA_API int Native_Sqlite3_step();
    LUA_API int Native_Sqlite3_column_count();
    LUA_API void Native_Sqlite3_column_text(int iCol, char* buffer, int* bufferSize);
    LUA_API void Native_Sqlite3_column_text_bufferindex(int iCol, char* buffer, int bufferindex, int* bufferSize);
    LUA_API int Native_Sqlite3_column_int(int iCol);
    LUA_API double Native_Sqlite3_column_double(int iCol);
    LUA_API void Native_Sqlite3_errmsg(char* buffer, int*  bufferSize);
    LUA_API void Native_Sqlite3_finalize();
#endif

}

#if USE_DECRYPTLUA
static std::string strEncryptKey = "";

LUA_API void NativeInitDecryptLua(const char *encryptKey, const char *sign)
{
	init_decryptlua_key_and_sign(encryptKey, sign);
}


LUA_API int  NativeDecryptLua( char *buff, int size,  char* decrypt,int compressType)
{
	unsigned char *decryptBuffer;
	size_t decryptSize = 0;
	if (decrypt_lua((unsigned char*)buff, size, &decryptBuffer, &decryptSize))
	{
		//strncpy(decrypt, (char*)decryptBuffer, decryptSize);
		//decrypt[decryptSize] = '/0';

		if (compressType == SNAPPY)
		{
			std::string output;
			if (snappy::Uncompress((const char*)decryptBuffer, decryptSize, &output))
			{
				memcpy(decrypt, output.c_str(), output.length());
				decryptSize = output.length();
			}
			else
			{
				memcpy(decrypt, decryptBuffer, decryptSize);
			}
		}	else if (compressType == ZIP)
		{
			//TODO
			memcpy(decrypt, decryptBuffer, decryptSize);
		}
		else
		{
			memcpy(decrypt, decryptBuffer, decryptSize);
		}
		free(decryptBuffer);
		return decryptSize;
	}
	else
	{
		return 0;
	}
}
#endif

#if USE_DBMGR
LUA_API void PCDBNativeSetEncryptKey(const char *encryptKey)
{
	auto pcdbMgr = gamebase::PCDBManager_v2::getInstance();
	pcdbMgr->setEncryptKey(encryptKey);
}

LUA_API void PCDBNativeSetReadonlyPath(const char *path)
{
	auto pcdbMgr = gamebase::PCDBManager_v2::getInstance();
	pcdbMgr->setReadonlyPath(path);
}

LUA_API void PCDBNativeSetWritablePath(const char *path)
{
	auto pcdbMgr = gamebase::PCDBManager_v2::getInstance();
	pcdbMgr->setWritablePath(path);
}

LUA_API void PCDBNativeSetDebugPathAndTarget(const char *path, const char *key, const char *value)
{
    auto pcdbMgr = gamebase::PCDBManager_v2::getInstance();
	pcdbMgr->setDebugPathAndTarget(path, key, value);
}

LUA_API void PCDBNativeInit()
{
    auto pcdbMgr = gamebase::PCDBManager_v2::getInstance();
	pcdbMgr->init();
}

// LUA_API void PCDBNativeResetVersionNote()
// {
//     auto pcdbMgr = gamebase::PCDBManager_v2::getInstance();
// 	pcdbMgr->resetVersionNote();
// }
LUA_API bool PCDBNativeIsFirstStart()
{
    auto pcdbMgr = gamebase::PCDBManager_v2::getInstance();
	return pcdbMgr->isFirstStart();
}
LUA_API bool PCDBNativeIsCoverInstall()
{
    auto pcdbMgr = gamebase::PCDBManager_v2::getInstance();
	return pcdbMgr->isCoverInstall();
}
LUA_API void PCDBNativeSearchURI(const char *reskey, char *buffer, int *bufferSize)
{   
    auto pcdbMgr = gamebase::PCDBManager_v2::getInstance();
    std::string ret = pcdbMgr->searchURI(reskey);
    assert(ret.length() < *bufferSize);
    strcpy(buffer, ret.c_str());
    *bufferSize = ret.length();
}

LUA_API bool PCDBNativeIsDevMode()
{   
    auto pcdbMgr = gamebase::PCDBManager_v2::getInstance();
	return pcdbMgr->isDevMode();
}


LUA_API void PCDBNativeGetGameConf(const char *key, char *buffer, int *bufferSize)
{   
    auto pcdbMgr = gamebase::PCDBManager_v2::getInstance();
    std::string ret = pcdbMgr->getGameConf(key);
    assert(ret.length() < *bufferSize);
    strcpy(buffer, ret.c_str());
    *bufferSize = ret.length();
}


LUA_API void Native_Sqlite3_finalize()
{
    gamebase::Sqlite3Manager::getInstance()->Sqlite3_finalize();
}

LUA_API bool PCDBNativeCurrEncryptCanOpen(const char *dbName)
{   
    auto pcdbMgr = gamebase::PCDBManager_v2::getInstance();
    return pcdbMgr->isCurrEncryptCanOpen(dbName);
}
#endif

#if defined(ANDROID)
#if USE_FileUtils
LUA_API void NativeSetJavaAssetManager(void* assetManager)
{
    FileUtils::getInstance()->SetAssetManager(assetManager);
}

LUA_API bool NativeFileExistInAssets(const char* path)
{
    return FileUtils::getInstance()->FileExistInAssets(path);
}

LUA_API int NativeGetFileSize(const char* path)
{
    return FileUtils::getInstance()->GetFileSize(path);
}

LUA_API void NativeGetFileContent(const char* path, char* buffer)
{
    FileUtils::getInstance()->ReadAssetsBytes(path, buffer);
}
#endif
#endif



static int l_ToluaInit(lua_State *L)
{
    toluafix_open(L);
    g_luaType.clear();
    lua_settop(L, 0);
    return 0;
}

static const struct luaL_Reg mylib [] = {
      {"Init", l_ToluaInit},
      {NULL, NULL}  /* sentinel */
    };

LUA_API int luaopen_Toluafix (lua_State *L) {
      luaL_openlib(L, "Toluafix", mylib, 0);
      return 1;
    }

#if USE_DECRYPTLUA
static std::string GetSqliteEncryptKey()
{
    return strEncryptKey;
    //unsigned char key[] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6};

    //return std::string(key, key + (sizeof(key)/sizeof(key[0])));
}
#endif

LUA_API void NativeInitAES(const char *cfg)
{
    if (cfg == NULL)
    {
        TJLOG("%s cfg is empty, not encrypt", __FUNCTION__);
        return;
    }
#if USE_HTTPREUEST
    unsigned char *buffer = nullptr;
    int len = cocos2d::base64Decode(reinterpret_cast<const unsigned char *>(cfg), (unsigned int)(strlen(cfg)), &buffer);
    CommunicationManager::instance()->setCrypto("aes", buffer, len);
    free(buffer);
    buffer = nullptr;
#endif
}

LUA_API void NativeExitGame()
{
#if USE_DBMGR
    gamebase::Sqlite3Manager::destroyInstance();
#endif
#if USE_HOTUPDATE
    gamebase::PCHotUpdate::destoryInstance();
#endif
#if USE_HTTPREUEST
    CommunicationManager::destoryInstance();
#endif
    removeAllFunctionsInUnityMainThread();
}

LUA_API void NativePerformFunctionInMainThread()
{
    performFunctionInUnityMainThread();
}



#if USE_DBMGR
LUA_API void NativeInitSqliteManager()
{
    auto sqlMgr = gamebase::Sqlite3Manager::getInstance();
}

LUA_API void NativeSetSqliteEncryptKey(const char* dbPathName, const char* encryptKey)
{
    gamebase::Sqlite3Manager::getInstance()->setDbEncryptKey(dbPathName, encryptKey);
}


LUA_API void NativeExcuteSqlite3(const char* sql, const char* dbName, char *buffer, int* bufferSize)
{
    std::string ret = gamebase::Sqlite3Manager::getInstance()->execute_sqlite3(sql, dbName);
    assert(ret.length() < *bufferSize);   
    strcpy(buffer, ret.c_str());
    *bufferSize = ret.length();
}

LUA_API int Native_Sqlite3_prepare(const char* dbName, const char* zSql)
{
    return gamebase::Sqlite3Manager::getInstance()->Sqlite3_prepare(dbName, zSql);
}
LUA_API int Native_Sqlite3_step()
{
    return gamebase::Sqlite3Manager::getInstance()->Sqlite3_step();
}
LUA_API int Native_Sqlite3_column_count()
{
    return gamebase::Sqlite3Manager::getInstance()->Sqlite3_column_count();
}
LUA_API void Native_Sqlite3_column_text(int iCol, char* buffer, int* bufferSize)
{
    std::string ret = gamebase::Sqlite3Manager::getInstance()->Sqlite3_column_text(iCol);
    assert(ret.length() < *bufferSize);   
    strcpy(buffer, ret.c_str());
    *bufferSize = ret.length();
}
LUA_API int Native_Sqlite3_column_int(int iCol)
{
    return gamebase::Sqlite3Manager::getInstance()->Sqlite3_column_int(iCol);
}
LUA_API double Native_Sqlite3_column_double(int iCol)
{
    return gamebase::Sqlite3Manager::getInstance()->Sqlite3_column_double(iCol);
}
LUA_API void Native_Sqlite3_errmsg(char* buffer, int* bufferSize)
{
    std::string ret = gamebase::Sqlite3Manager::getInstance()->Sqlite3_errmsg();
    assert(ret.length() < *bufferSize);   
    strcpy(buffer, ret.c_str());
    *bufferSize = ret.length();
}
LUA_API void Native_Sqlite3_column_text_bufferindex(int iCol, char* buffer, int bufferindex, int* bufferSize)
{
    std::string ret = gamebase::Sqlite3Manager::getInstance()->Sqlite3_column_text(iCol);
    int length = ret.length();
    assert(length < (*bufferSize-bufferindex));
    strncpy(buffer+bufferindex, ret.c_str(), length);
    *bufferSize = length;
}
#endif