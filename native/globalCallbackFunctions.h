#ifndef GLOBAL_CALLBACK_FUNCTIONS_H
#define GLOBAL_CALLBACK_FUNCTIONS_H

struct LoadImageRequest {
    double time;
    int width;
    int height;
    char* data;
};

typedef void(*LoadImageCallback)(LoadImageRequest*);

struct GlobalFunctions {
    LoadImageCallback loadImageCallback;
};

extern GlobalFunctions* globalFunctionPointers;

#endif