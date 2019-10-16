#ifndef PARAM_SET_H
#define PARAM_SET_H
#include <vector>
#include "../openfx/include/ofxProperty.h"
#include "../openfx/include/ofxImageEffect.h"

struct OfxParamStruct {
    int paramId;
    const char* name;
    const char* type;
    OfxPropertySetHandle properties;
    
    OfxImageEffectHandle imageEffectHandle;

    OfxParamStruct(const char* name, const char* type);
    ~OfxParamStruct();
};

struct OfxParamSetStruct {
    std::vector<OfxParamStruct*> parameters;

    ~OfxParamSetStruct();
};

#include "../openfx/include/ofxParam.h"

OfxStatus paramDefine(OfxParamSetHandle paramSet,
            const char *paramType,
            const char *name,
            OfxPropertySetHandle *propertySet);
OfxStatus paramGetHandle(OfxParamSetHandle paramSet,
                const char *name,
                OfxParamHandle *param,
                OfxPropertySetHandle *propertySet);
OfxStatus paramSetGetPropertySet(OfxParamSetHandle paramSet, OfxPropertySetHandle *propHandle);
OfxStatus paramGetPropertySet(OfxParamHandle param, OfxPropertySetHandle *propHandle);
OfxStatus paramGetValue(OfxParamHandle  paramHandle, ...);
OfxStatus paramGetValueAtTime(OfxParamHandle  paramHandle, OfxTime time, ...);
OfxStatus paramGetDerivative(OfxParamHandle  paramHandle, OfxTime time, ...);
OfxStatus paramGetIntegral(OfxParamHandle  paramHandle, OfxTime time1, OfxTime time2, ...);
OfxStatus paramSetValue(OfxParamHandle  paramHandle, ...);
OfxStatus paramSetValueAtTime(OfxParamHandle  paramHandle, OfxTime time, ...);
OfxStatus paramGetNumKeys(OfxParamHandle  paramHandle, unsigned int  *numberOfKeys);
OfxStatus paramGetKeyTime(OfxParamHandle  paramHandle, unsigned int nthKey, OfxTime *time);
OfxStatus paramGetKeyIndex(OfxParamHandle  paramHandle,
            OfxTime time,
            int     direction,
            int    *index);
OfxStatus paramDeleteKey(OfxParamHandle  paramHandle, OfxTime time);
OfxStatus paramDeleteAllKeys(OfxParamHandle  paramHandle);
OfxStatus paramCopy(OfxParamHandle  paramTo, OfxParamHandle  paramFrom, OfxTime dstOffset, const OfxRangeD *frameRange);
OfxStatus paramEditBegin(OfxParamSetHandle paramSet, const char *name); 
OfxStatus paramEditEnd(OfxParamSetHandle paramSet);

OfxParameterSuiteV1* getParameterSuite();

#endif