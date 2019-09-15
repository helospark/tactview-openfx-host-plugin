#ifndef IMAGE_EFFECT_STRUCT
#define IMAGE_EFFECT_STRUCT
#include "../openfx/include/ofxCore.h"
#include "../openfx/include/ofxParam.h"
#include "ofx_property.h"

struct OfxImageClipStruct {
    OfxPropertySetHandle properties;
    void* data = NULL;
    char* type;
};

struct OfxImageEffectStruct {
    OfxPropertySetHandle properties;
    std::map<std::string, OfxImageClipStruct*> clips;
    OfxParamSetHandle parameters;
    
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
int abort(OfxImageEffectHandle imageEffect);
OfxStatus imageMemoryAlloc(OfxImageEffectHandle instanceHandle, 
            size_t nBytes,
            OfxImageMemoryHandle *memoryHandle);
OfxStatus imageMemoryFree(OfxImageMemoryHandle memoryHandle);
OfxStatus imageMemoryLock(OfxImageMemoryHandle memoryHandle,
                void **returnedPtr);
OfxStatus imageMemoryUnlock(OfxImageMemoryHandle memoryHandle);

OfxImageEffectSuiteV1* getOfxImageEffectSuiteV1();

#endif