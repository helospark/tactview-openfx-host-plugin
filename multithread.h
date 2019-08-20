struct OfxMutex {

};

#include "openfx/include/ofxMultiThread.h"

OfxStatus multiThread(OfxThreadFunctionV1 func, unsigned int nThreads, void *customArg);
OfxStatus multiThreadNumCPUs(unsigned int *nCPUs);
OfxStatus multiThreadIndex(unsigned int *threadIndex);
int multiThreadIsSpawnedThread(void);
OfxStatus mutexCreate(OfxMutexHandle *mutex, int lockCount);
OfxStatus mutexDestroy(const OfxMutexHandle mutex);
OfxStatus mutexLock(const OfxMutexHandle mutex);
OfxStatus mutexUnLock(const OfxMutexHandle mutex);
OfxStatus mutexTryLock(const OfxMutexHandle mutex);

OfxMultiThreadSuiteV1* getMultiThreadSuite();