#include "multithread.h"
#include <iostream>

OfxMultiThreadSuiteV1* multithreadSuite = NULL;

OfxStatus multiThread(OfxThreadFunctionV1 func, unsigned int nThreads, void *customArg) {
    std::cout << "multiThread" << std::endl;
    func(0, 1, customArg);
    return kOfxStatOK;
}

OfxStatus multiThreadNumCPUs(unsigned int *nCPUs) {
    std::cout << "multiThreadNumCPUs" << std::endl;
    *nCPUs = 1;
    return kOfxStatOK;
}

OfxStatus multiThreadIndex(unsigned int *threadIndex) {
    std::cout << "multiThreadIndex" << std::endl;
    *threadIndex = 0;
    return kOfxStatOK;
}

int multiThreadIsSpawnedThread(void) {
    std::cout << "[!ERROR!] multiThreadIsSpawnedThread" << std::endl;
    return kOfxStatOK;
}

OfxStatus mutexCreate(OfxMutexHandle *mutex, int lockCount) {
    std::cout << "mutexCreate" << std::endl;

    *mutex = new OfxMutex();

    return kOfxStatOK;
}

OfxStatus mutexDestroy(const OfxMutexHandle mutex) {
    std::cout << "mutexDestroy" << std::endl;
    return kOfxStatOK;
}

OfxStatus mutexLock(const OfxMutexHandle mutex) {
    //std::cout << "[!ERROR!] mutexLock" << std::endl;

    return kOfxStatOK;
}
OfxStatus mutexUnLock(const OfxMutexHandle mutex) {
    //std::cout << "[!ERROR!] mutexUnLock" << std::endl;
    return kOfxStatOK;
}

OfxStatus mutexTryLock(const OfxMutexHandle mutex){
    std::cout << "mutexTryLock" << std::endl;
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