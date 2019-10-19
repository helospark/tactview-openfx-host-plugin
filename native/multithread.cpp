#include "multithread.h"
#include <iostream>
#include "global.h"

OfxMultiThreadSuiteV1* multithreadSuite = NULL;

OfxStatus multiThread(OfxThreadFunctionV1 func, unsigned int nThreads, void *customArg) {
    LOG("multiThread" );
    func(0, 1, customArg);
    return kOfxStatOK;
}

OfxStatus multiThreadNumCPUs(unsigned int *nCPUs) {
    LOG("multiThreadNumCPUs" );
    *nCPUs = 1;
    return kOfxStatOK;
}

OfxStatus multiThreadIndex(unsigned int *threadIndex) {
    LOG("multiThreadIndex" );
    *threadIndex = 0;
    return kOfxStatOK;
}

int multiThreadIsSpawnedThread(void) {
    LOG_ERROR("multiThreadIsSpawnedThread" );
    return kOfxStatOK;
}

OfxStatus mutexCreate(OfxMutexHandle *mutex, int lockCount) {
    LOG("mutexCreate" );

    *mutex = new OfxMutex();

    return kOfxStatOK;
}

OfxStatus mutexDestroy(const OfxMutexHandle mutex) {
    LOG("mutexDestroy" );
    return kOfxStatOK;
}

OfxStatus mutexLock(const OfxMutexHandle mutex) {
    return kOfxStatOK;
}
OfxStatus mutexUnLock(const OfxMutexHandle mutex) {
    return kOfxStatOK;
}

OfxStatus mutexTryLock(const OfxMutexHandle mutex){
    LOG("mutexTryLock" );
    return kOfxStatOK;
}

OfxMultiThreadSuiteV1* getMultiThreadSuite() {
    if (multithreadSuite != NULL) {
        return multithreadSuite;
    } else {
        multithreadSuite = new OfxMultiThreadSuiteV1();
        multithreadSuite->multiThread = &multiThread;
        multithreadSuite->multiThreadIndex = &multiThreadIndex;
        multithreadSuite->multiThreadIsSpawnedThread = &multiThreadIsSpawnedThread;
        multithreadSuite->multiThreadNumCPUs = &multiThreadNumCPUs;
        multithreadSuite->mutexCreate = &mutexCreate;
        multithreadSuite->mutexDestroy = &mutexDestroy;
        multithreadSuite->mutexLock = &mutexLock;
        multithreadSuite->mutexTryLock = &mutexTryLock;
        multithreadSuite->mutexUnLock = &mutexUnLock;
        return multithreadSuite;
    }
}