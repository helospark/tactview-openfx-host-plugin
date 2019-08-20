#include "openfx/include/ofxCore.h"
#include "openfx/include/ofxMemory.h"

OfxStatus memoryAlloc(void *handle, 
            size_t nBytes,
            void **allocatedData);
OfxStatus memoryFree(void *allocatedData);

OfxMemorySuiteV1* getMemorySuiteV1();