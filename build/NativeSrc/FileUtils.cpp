#include "FileUtils.h"


FileUtils::FileUtils()
{

}

FileUtils::~FileUtils()
{
    
}

FileUtils* FileUtils::_instance = nullptr;
FileUtils* FileUtils::getInstance()
{
	if (_instance == nullptr) {
        _instance = new FileUtils();
    }
    return _instance;
}

void FileUtils::destroyInstance()
{

}

#ifdef ANDROID

JNIEnv* jni_env = NULL;
JavaVM* JVM;
extern "C" 
{
    JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
    {
        JVM = vm;
        if (vm->GetEnv((void**)&jni_env, JNI_VERSION_1_6) != JNI_OK)
            __android_log_print(ANDROID_LOG_ERROR, "Unity", "ERROR: GetEnv failed");

        return JNI_VERSION_1_6;
    }
}

void FileUtils::SetAssetManager(void* assetMgr)
{
    TJLOG("~~~ set asset manager from java");
    if (nullptr == assetMgr)
    {
        TJLOG("setassetmanager : received unexpected nullptr parameter");
        return;
    }
    assetManager = AAssetManager_fromJava(jni_env, (jobject)assetMgr);
};

// extern "C" {
//     JNIEXPORT void JNICALL Java_com_utils_core_NativeHelper_SetAssetManager(JNIEnv *env, jobject thiz, jobject assetMgr)
//     {
//         TJLOG("~~~ set asset manager from java");
//         if (nullptr == assetMgr) {
//             TJLOG("setassetmanager : received unexpected nullptr parameter");
//             return;
//         }
//         FileUtils::getInstance()->SetAssetManager(AAssetManager_fromJava(env, assetMgr));
//     }
// }

bool FileUtils::FileExistInAssets(const char* fileName)
{
    if(assetManager == nullptr){
        TJLOG("~~~ assetManager is null");
        return false;
    }
    std::string path = GetRelativePathInAssets(fileName);
    AAsset* asset = AAssetManager_open(assetManager, path.c_str(), AASSET_MODE_UNKNOWN);
    if(asset == nullptr){
        TJLOG("~~~ AAssetManager_open returns nullptr");
        return false;
    }
    AAsset_close(asset);
    return true;
}

int32_t FileUtils::GetFileSize(const char* fileName)
{
    if(assetManager == nullptr){
        TJLOG("~~~ assetManager is null");
        return -1;
    }
    std::string path = GetRelativePathInAssets(fileName);
    AAsset* asset = AAssetManager_open(assetManager, path.c_str(), AASSET_MODE_UNKNOWN);
    if(asset == nullptr){
        TJLOG("~~~ AAssetManager_open returns nullptr");
        return -1;
    }
   
    size_t fileLength = AAsset_getLength(asset);
    AAsset_close(asset);
    return fileLength;
}

void FileUtils::ReadAssetsBytes(const char* fileName, char* bytes)
{
    if(assetManager == nullptr){
        TJLOG("~~~ assetManager is null");
        return;
    }
    std::string path = GetRelativePathInAssets(fileName);
    AAsset* asset = AAssetManager_open(assetManager, path.c_str(), AASSET_MODE_UNKNOWN);
    if(asset == nullptr){
        TJLOG("~~~ AAssetManager_open returns nullptr");
        return;
    }
    size_t size = AAsset_getLength(asset);
    const char *buffer = (const char *) AAsset_getBuffer(asset);
    memcpy(bytes, buffer, size);
    AAsset_close(asset);
}

std::string FileUtils::GetRelativePathInAssets(std::string fullPath)
{
    // input: jar:file:///data/app/com.topjoy.u3d-2/base.apk!/assets/game.conf
    // output: game.conf
    std::string path;
    size_t pos = fullPath.find("!/assets/");
    if (pos != std::string::npos) {
        path = fullPath.substr(pos + 9);
    }
    else
    {
        path = fullPath;
    }
    return path;
}

#endif
