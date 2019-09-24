#ifndef GLOBAL_CALLBACK_FUNCTIONS_H
#define GLOBAL_CALLBACK_FUNCTIONS_H

struct LoadImageRequest {
    double time;
    int width;
    int height;
    char* data;
};

struct ResolvedParameterStructure {
    int intValue1;
    int intValue2;
    int intValue3;
    double doubleValue1;
    double doubleValue3;
    double doubleValue2;
    double doubleValue4;
};

struct ResolveValueRequest {
    int uniqueId;
    double time;

    ResolvedParameterStructure* result;
};

typedef void(*LoadImageCallback)(LoadImageRequest*);
typedef void(*ParameterValueProviderCallback)(ResolveValueRequest*);

struct GlobalFunctions {
    LoadImageCallback loadImageCallback;
    ParameterValueProviderCallback resolveParameterCallback;
};

extern GlobalFunctions* globalFunctionPointers;

#endif