#include "memorySuite.h"
#include <iostream>
#include "global.h"

OfxMemorySuiteV1* memorySuite = NULL;

OfxStatus memoryAlloc(void *handle, 
            size_t nBytes,
            void **allocatedData) {
    LOG("memoryAlloc" );
                *allocatedData = new char[nBytes];
                return kOfxStatOK;
}

OfxStatus memoryFree(void *allocatedData) {
    LOG("memoryFree" );
    delete[] (char*)allocatedData;
    return kOfxStatOK;
}

OfxMemorySuiteV1* getMemorySuiteV1() {
    if (memorySuite != NULL) {
        return memorySuite;
    } else {
        memorySuite = new OfxMemorySuiteV1();
        memorySuite->memoryAlloc = &memoryAlloc;
        memorySuite->memoryFree = &memoryFree;
        return memorySuite;
    }
}