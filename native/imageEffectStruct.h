#ifndef IMAGE_EFFECT_STRUCT
#define IMAGE_EFFECT_STRUCT
#include "../openfx/include/ofxCore.h"
#include "../openfx/include/ofxParam.h"
#include "ofx_property.h"
#include "imageLoader.h"

typedef struct OfxImageEffectStruct *OfxImageEffectHandle;

struct CurrentRenderRequest {
    int width;
    int height;
    std::map<std::string, Image*> sourceClips;

    // only for transitions
    double transitionProgress;
};

struct OfxImageClipStruct {
    OfxPropertySetHandle properties;
    void* data = NULL;
    int dataSize = 0;
    bool allocated = false;
    char* type;
    OfxImageEffectHandle imageEffect;
};

struct OfxImageEffectStruct {
    OfxPropertySetHandle properties;
    std::map<std::string, OfxImageClipStruct*> clips;
    OfxParamSetHandle parameters;
    char* effectId;

    // getClipImage does not get current inParams, so we have to store this state here
    // making parallelization impossible :(
    CurrentRenderRequest* currentRenderRequest;

    // TODO: pointer type
    void* describeInContextList;
    void* clipList;
    
    OfxImageEffectStruct();
};

#include "../openfx/include/ofxImageEffect.h"

OfxStatus getPropertySet(OfxImageEffectHandle imageEffect,
                OfxPropertySetHandle *propHandle);
OfxStatus getParamSet(OfxImageEffectHandle imageEffect,
            OfxParamSetHandle *paramSet);
OfxStatus clipDefine(OfxImageEffectHandle imageEffect,
            const char *name,	 
            OfxPropertySetHandle *propertySet);
OfxStatus clipGetHandle(OfxImageEffectHandle imageEffect,
                const char *name,
                OfxImageClipHandle *clip,
                OfxPropertySetHandle *propertySet);
OfxStatus clipGetPropertySet(OfxImageClipHandle clip,
                OfxPropertySetHandle *propHandle);
OfxStatus clipGetImage(OfxImageClipHandle clip,
            OfxTime       time,
            const OfxRectD     *region,
            OfxPropertySetHandle   *imageHandle);
OfxStatus clipReleaseImage(OfxPropertySetHandle imageHandle);
OfxStatus clipGetRegionOfDefinition(OfxImageClipHandle clip,
                    OfxTime time,
                    OfxRectD *bounds);
int abortInternal(OfxImageEffectHandle imageEffect);
OfxStatus imageMemoryAlloc(OfxImageEffectHandle instanceHandle, 
            size_t nBytes,
            OfxImageMemoryHandle *memoryHandle);
OfxStatus imageMemoryFree(OfxImageMemoryHandle memoryHandle);
OfxStatus imageMemoryLock(OfxImageMemoryHandle memoryHandle,
                void **returnedPtr);
OfxStatus imageMemoryUnlock(OfxImageMemoryHandle memoryHandle);

OfxImageEffectSuiteV1* getOfxImageEffectSuiteV1();

#endif