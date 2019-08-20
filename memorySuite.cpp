#include "memorySuite.h"
#include <iostream>

OfxMemorySuiteV1* memorySuite = NULL;

OfxStatus memoryAlloc(void *handle, 
            size_t nBytes,
            void **allocatedData) {
    std::cout << "memoryAlloc" << std::endl;
                *allocatedData = new char[nBytes];
                return kOfxStatOK;
}

OfxStatus memoryFree(void *allocatedData) {
    std::cout << "memoryFree" << std::endl;
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