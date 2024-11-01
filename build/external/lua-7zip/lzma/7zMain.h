
#ifndef testLzma__zMain_h
#define testLzma__zMain_h

typedef void Progress7z(int curFileCount, int totalFileCount);

#ifdef __cplusplus
extern "C" {
#endif
    
    int main7z(int numargs, char *args[],  Progress7z progress7z);
    
#ifdef __cplusplus
}
#endif

#endif
