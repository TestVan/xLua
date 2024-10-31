#pragma once

#include <stdint.h>
#include <string>
#include "tjConfig.h"

#ifdef ANDROID
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>
#include <android/log.h>
#include <new>
#include <stdio.h>
#endif

class FileUtils
{
public:
    FileUtils();
    ~FileUtils();
    static FileUtils* getInstance();
    static void destroyInstance();

#ifdef ANDROID
    bool FileExistInAssets(const char* path);

    int32_t GetFileSize(const char* path);

    // const char* ReadAssetsBytes(const char* path);
    void ReadAssetsBytes(const char* fileName, char* bytes);

    void SetAssetManager(void* assetMgr);
#endif

private:
    static FileUtils* _instance;

#ifdef ANDROID
    AAssetManager *assetManager = nullptr;

    std::string GetRelativePathInAssets(std::string fullPath);

#endif

};
