#include "paramSet.h"
#include <cstring>
#include <cstdarg>
#include "ofx_property.h"
#include "globalCallbackFunctions.h"
#include "imageEffectStruct.h"
#include "string_operations.h"
#include "global.h"

OfxParameterSuiteV1* parameterSuite = NULL;

OfxParamStruct::OfxParamStruct(const char* name, const char* type) {
    this->name = duplicateString(name);
    this->type = duplicateString(type);
    this->properties = new OfxPropertySetStruct();
}

OfxParamStruct::~OfxParamStruct() {
    delete properties;
    delete[] name;
    delete[] type;
}

OfxParamSetStruct::~OfxParamSetStruct() {
    for (auto e : this->parameters) {
        delete e;
    }
}

char* copyString(const char* str) {
    int len = strlen(str);
    char* newPointer = new char[len + 1];
    strcpy(newPointer, str);
    newPointer[len] = '\0';
    return newPointer;
}

OfxStatus paramDefine(OfxParamSetHandle paramSet,
            const char *paramType,
            const char *name,
            OfxPropertySetHandle *propertySet) {
                LOG("paramDefine " << name );

                OfxParamHandle result = new OfxParamStruct(name, paramType); 

                propSetString(result->properties, kOfxParamPropType, 0, paramType);
                propSetString(result->properties, kOfxPropName, 0, name);
                result->paramId = -1; // set to real value during createInstance

                paramSet->parameters.push_back(result);

                if (strcmp(paramType, kOfxParamTypeDouble2D) == 0) {
                    propSetString(result->properties, kOfxParamPropDefaultCoordinateSystem, 0, kOfxParamDoubleTypeXY);
                }

                if (propertySet != NULL) {
                    *propertySet = result->properties;
                }


                return kOfxStatOK;
}

OfxStatus paramGetHandle(OfxParamSetHandle paramSet,
                const char *name,
                OfxParamHandle *param,
                OfxPropertySetHandle *propertySet) {
                    LOG("paramGetHandle " << name << " " << param );
                    for (int i = 0; i < paramSet->parameters.size(); ++i) {
                        if (strcmp(paramSet->parameters[i]->name, name) == 0) {
                            *param = paramSet->parameters[i];
                            if (propertySet != NULL) {
                                *propertySet = paramSet->parameters[i]->properties;
                            }
                            return kOfxStatOK;
                        }
                    }
                    LOG("[!Error!] - " << name  << " not found" );
                    for (int i = 0; i < paramSet->parameters.size(); ++i) {
                        LOG("   -  " <<  paramSet->parameters[i]->name );
                    }
                    return kOfxStatErrBadHandle;
                }

OfxStatus paramSetGetPropertySet(OfxParamSetHandle paramSet, OfxPropertySetHandle *propHandle) {
        // WHUT IS THIS??
    *propHandle = NULL;

    LOG_ERROR("paramSetGetPropertySet" );
    return kOfxStatOK;

}

OfxStatus paramGetPropertySet(OfxParamHandle param, OfxPropertySetHandle *propHandle) {
    LOG("paramGetPropertySet" );
    *propHandle = param->properties;
    return kOfxStatOK;
}

bool isNormalized(OfxPropertySetHandle properties) {
    auto stringIterator = properties->strings.find(kOfxParamPropDefaultCoordinateSystem);
    if (stringIterator == properties->strings.end()) {
        return false;
    }
    return !strcmp(stringIterator->second[0], kOfxParamCoordinatesNormalised);
}

OfxStatus paramGetValue(OfxParamHandle  paramHandle, ...) {
    LOG("paramGetValue " << paramHandle->type << " " << paramHandle->name );
    
#ifdef DEBUG
    LOG("paramGetValue " << paramHandle->type << " " << paramHandle->name );
    
    va_list ap;
    va_start(ap, paramHandle);

    if (strcmp(paramHandle->type, kOfxParamTypeInteger) == 0) {
        int* value = va_arg(ap, int*);
        *value = paramHandle->properties->integers[kOfxParamPropDefault][0];
        LOG(" returned " << *value );
    } else if (strcmp(paramHandle->type, kOfxParamTypeDouble) == 0) {
        double* value = va_arg(ap, double*);
        *value = paramHandle->properties->doubles[kOfxParamPropDefault][0];
        LOG(" returned " << *value );
    } else if (strcmp(paramHandle->type, kOfxParamTypeBoolean) == 0) {
        int* value = va_arg(ap, int*);
        if (paramHandle->properties->integers.find(kOfxParamPropDefault) != paramHandle->properties->integers.end()) {
            *value = paramHandle->properties->integers[kOfxParamPropDefault][0];
        } else {
            *value = 0;
        }
        //*value = 1;
        LOG(" returned " << *value );
    } else if (strcmp(paramHandle->type, kOfxParamTypeInteger2D)== 0) {
        int* value = va_arg(ap, int*);
        *value = paramHandle->properties->integers[kOfxParamPropDefault][0];
        int* value2 = va_arg(ap, int*);
        *value2 = paramHandle->properties->integers[kOfxParamPropDefault][1];
        LOG("Integer2D returned " << *value << " " << *value2 );
    } else if (strcmp(paramHandle->type, kOfxParamTypeDouble2D) == 0) {
        double* value = va_arg(ap, double*);
        *value = paramHandle->properties->doubles[kOfxParamPropDefault][0];
        double* value2 = va_arg(ap, double*);
        *value2 = paramHandle->properties->doubles[kOfxParamPropDefault][1];
        LOG("Double2D returned " << *value << " " << *value2 );
    }else if (strcmp(paramHandle->type, kOfxParamTypeDouble3D) == 0) {
        for (int i = 0; i < 3; ++i) {
            double* value = va_arg(ap, double*);
            *value = paramHandle->properties->doubles[kOfxParamPropDefault][0];
            LOG("double3d[" << i << "] = " << *value );
        }
    }  else if (strcmp(paramHandle->type, kOfxParamTypeRGBA) == 0) {
        for (int i = 0; i < 4; ++i) {
            double* value = va_arg(ap, double*);
            *value = paramHandle->properties->doubles[kOfxParamPropDefault][i];
            LOG("RGBA[" << i << "] = " << *value );
        }
    } else if (strcmp(paramHandle->type, kOfxParamTypeRGB) == 0) {
        for (int i = 0; i < 3; ++i) {
            double* value = va_arg(ap, double*);
            *value = paramHandle->properties->doubles[kOfxParamPropDefault][i];
            LOG("RGB[" << i << "] = " << *value );
        }
    } else if (strcmp(paramHandle->type, kOfxParamTypeChoice) == 0) {
        int* value = va_arg(ap, int*);
        auto val = paramHandle->properties->integers[kOfxParamPropDefault];
        if (val.size() > 0) {
            *value = val[0];
        } else {
            *value = 0;
        }
        LOG(" returned " << *value );
    } else {
        LOG("[!ERROR!] Type not supported yet " << paramHandle->type );
    }
    return kOfxStatOK;
#else

    va_list ap;
    va_start(ap, paramHandle);

    if (strcmp(paramHandle->name, kOfxImageEffectTransitionParamName) == 0) {
        double* value = va_arg(ap, double*);
        *value = paramHandle->imageEffectHandle->currentRenderRequest->transitionProgress;
        LOG("TransitionProgress " << *value );
        return kOfxStatOK;
    }

    ResolveValueRequest resolveValueRequest;
    resolveValueRequest.uniqueId = paramHandle->paramId;
    resolveValueRequest.time = 0.0;
    resolveValueRequest.result = new ResolvedParameterStructure();

    globalFunctionPointers->resolveParameterCallback(&resolveValueRequest);

    if (strcmp(paramHandle->type, kOfxParamTypeInteger) == 0) {
        int* value = va_arg(ap, int*);
        *value = resolveValueRequest.result->intValue1;
        LOG(" returned " << *value );
    } else if (strcmp(paramHandle->type, kOfxParamTypeDouble) == 0) {
        double* value = va_arg(ap, double*);
        *value = resolveValueRequest.result->doubleValue1;
        LOG(" returned " << *value );
    } else if (strcmp(paramHandle->type, kOfxParamTypeBoolean) == 0) {
        int* value = va_arg(ap, int*);
        *value = resolveValueRequest.result->intValue1;
        LOG(" returned " << *value );
    } else if (strcmp(paramHandle->type, kOfxParamTypeInteger3D)== 0) {
        int* value = va_arg(ap, int*);
        double* value1 = va_arg(ap, double*);
        double* value2 = va_arg(ap, double*);
        double* value3 = va_arg(ap, double*);
        *value1 = resolveValueRequest.result->intValue1;
        *value2 = resolveValueRequest.result->intValue2;
        *value3 = resolveValueRequest.result->intValue3;
    } else if (strcmp(paramHandle->type, kOfxParamTypeDouble2D) == 0) {
        double* value1 = va_arg(ap, double*);
        double* value2 = va_arg(ap, double*);
        *value1 = resolveValueRequest.result->doubleValue1;
        *value2 = resolveValueRequest.result->doubleValue2;

        if (isNormalized(paramHandle->properties)) {
            *value1 *= paramHandle->imageEffectHandle->currentRenderRequest->width;
            *value2 *= paramHandle->imageEffectHandle->currentRenderRequest->height;
        }
        LOG("Returned " << *value1 << " " << *value2 );
    }else if (strcmp(paramHandle->type, kOfxParamTypeDouble3D) == 0) {
        double* value1 = va_arg(ap, double*);
        double* value2 = va_arg(ap, double*);
        double* value3 = va_arg(ap, double*);
        *value1 = resolveValueRequest.result->doubleValue1;
        *value2 = resolveValueRequest.result->doubleValue2;
        *value3 = resolveValueRequest.result->doubleValue3;
    }  else if (strcmp(paramHandle->type, kOfxParamTypeRGBA) == 0) {
        double* value1 = va_arg(ap, double*);
        double* value2 = va_arg(ap, double*);
        double* value3 = va_arg(ap, double*);
        double* value4 = va_arg(ap, double*);
        *value1 = resolveValueRequest.result->doubleValue1;
        *value2 = resolveValueRequest.result->doubleValue2;
        *value3 = resolveValueRequest.result->doubleValue3;
        *value4 = resolveValueRequest.result->doubleValue4;
    } else if (strcmp(paramHandle->type, kOfxParamTypeRGB) == 0) {
        double* value1 = va_arg(ap, double*);
        double* value2 = va_arg(ap, double*);
        double* value3 = va_arg(ap, double*);
        *value1 = resolveValueRequest.result->doubleValue1;
        *value2 = resolveValueRequest.result->doubleValue2;
        *value3 = resolveValueRequest.result->doubleValue3;
    } else if (strcmp(paramHandle->type, kOfxParamTypeChoice) == 0) {
        int* value = va_arg(ap, int*);
        *value = resolveValueRequest.result->intValue1;
        LOG(" returned " << *value );
    } else {
        LOG("[!ERROR!] Type not supported yet " << paramHandle->type );
    }
    delete resolveValueRequest.result;

    return kOfxStatOK;
#endif
}

OfxStatus paramGetValueAtTime(OfxParamHandle  paramHandle, OfxTime time, ...) {

#ifdef DEBUG
    va_list ap;
    va_start(ap, time);

    if (strcmp(paramHandle->type, kOfxParamTypeInteger) == 0) {
        int* value = va_arg(ap, int*);
        if (paramHandle->properties->integers.find(kOfxParamPropDefault) != paramHandle->properties->integers.end()) {
            *value = paramHandle->properties->integers[kOfxParamPropDefault][0];
        } else {
            *value = paramHandle->properties->integers[kOfxParamPropDisplayMin][0];
        }
    } else if (strcmp(paramHandle->type, kOfxParamTypeDouble) == 0) {
        double* value = va_arg(ap, double*);
        *value = paramHandle->properties->doubles[kOfxParamPropDefault][0];
    } else if (strcmp(paramHandle->type, kOfxParamTypeDouble3D) == 0) {
        for (int i = 0; i < 3; ++i) {
            double* value = va_arg(ap, double*);
            *value = paramHandle->properties->doubles[kOfxParamPropDefault][0];
            LOG("double3d[" << i << "] = " << *value );
        }
    } else if (strcmp(paramHandle->type, kOfxParamTypeBoolean) == 0) {
        int* value = va_arg(ap, int*);
        *value = strstr(paramHandle->name, "NatronOfxParamProcess") != 0;
        LOG("   returns " << *value );
    } else if (strcmp(paramHandle->type, kOfxParamTypeRGBA) == 0) {
        for (int i = 0; i < 4; ++i) {
            double* value = va_arg(ap, double*);
            *value = paramHandle->properties->doubles[kOfxParamPropDefault][i];
            LOG("RGBA[" << i << "] = " << *value );
        }
    } else if (strcmp(paramHandle->type, kOfxParamTypeRGB) == 0) {
        for (int i = 0; i < 3; ++i) {
            double* value = va_arg(ap, double*);
            *value = paramHandle->properties->doubles[kOfxParamPropDefault][i];
            LOG("RGB[" << i << "] = " << *value );
        }
    } else if (strcmp(paramHandle->type, kOfxParamTypeChoice) == 0) {
        int* value = va_arg(ap, int*);
        auto val = paramHandle->properties->integers[kOfxParamPropDefault];
        if (val.size() > 0) {
            *value = val[0];
        } else {
            *value = 0;
        }
        LOG(" returned " << *value );
    } else if (strcmp(paramHandle->type, kOfxParamTypeDouble2D) == 0) {
        for (int i = 0; i < 2; ++i) {
            double* value = va_arg(ap, double*);
            *value = paramHandle->properties->doubles[kOfxParamPropDefault][i];
            LOG("double2d[" << i << "] = " << *value );
        }
    } else {
        LOG("[!ERROR!] Type not supported yet " << paramHandle->type );
    }
    return kOfxStatOK;
#else
    LOG("paramGetValueAtTime " << paramHandle->name );
    va_list ap;
    va_start(ap, time);

    if (strcmp(paramHandle->name, kOfxImageEffectTransitionParamName) == 0) {
        double* value = va_arg(ap, double*);
        *value = paramHandle->imageEffectHandle->currentRenderRequest->transitionProgress;
        LOG("TransitionProgress " << *value );
        return kOfxStatOK;
    }

    ResolveValueRequest resolveValueRequest;
    resolveValueRequest.uniqueId = paramHandle->paramId;
    resolveValueRequest.time = time;
    resolveValueRequest.result = new ResolvedParameterStructure();

    globalFunctionPointers->resolveParameterCallback(&resolveValueRequest);

    if (strcmp(paramHandle->type, kOfxParamTypeInteger) == 0) {
        int* value = va_arg(ap, int*);
        *value = resolveValueRequest.result->intValue1;
        LOG(" returned " << *value );
    } else if (strcmp(paramHandle->type, kOfxParamTypeDouble) == 0) {
        double* value = va_arg(ap, double*);
        *value = resolveValueRequest.result->doubleValue1;
        LOG(" returned " << *value );
    } else if (strcmp(paramHandle->type, kOfxParamTypeBoolean) == 0) {
        int* value = va_arg(ap, int*);
        *value = resolveValueRequest.result->intValue1;
        LOG(" returned " << *value );
    } else if (strcmp(paramHandle->type, kOfxParamTypeInteger3D)== 0) {
        int* value = va_arg(ap, int*);
        double* value1 = va_arg(ap, double*);
        double* value2 = va_arg(ap, double*);
        double* value3 = va_arg(ap, double*);
        *value1 = resolveValueRequest.result->intValue1;
        *value2 = resolveValueRequest.result->intValue2;
        *value3 = resolveValueRequest.result->intValue3;
    } else if (strcmp(paramHandle->type, kOfxParamTypeDouble2D) == 0) {
        double* value1 = va_arg(ap, double*);
        double* value2 = va_arg(ap, double*);
        *value1 = resolveValueRequest.result->doubleValue1;
        *value2 = resolveValueRequest.result->doubleValue2;

        if (isNormalized(paramHandle->properties)) {
            *value1 *= paramHandle->imageEffectHandle->currentRenderRequest->width;
            *value2 *= paramHandle->imageEffectHandle->currentRenderRequest->height;
        }

        LOG("returned " << *value1 << " " << *value2 );
    }else if (strcmp(paramHandle->type, kOfxParamTypeDouble3D) == 0) {
        double* value1 = va_arg(ap, double*);
        double* value2 = va_arg(ap, double*);
        double* value3 = va_arg(ap, double*);
        *value1 = resolveValueRequest.result->doubleValue1;
        *value2 = resolveValueRequest.result->doubleValue2;
        *value3 = resolveValueRequest.result->doubleValue3;
    }  else if (strcmp(paramHandle->type, kOfxParamTypeRGBA) == 0) {
        double* value1 = va_arg(ap, double*);
        double* value2 = va_arg(ap, double*);
        double* value3 = va_arg(ap, double*);
        double* value4 = va_arg(ap, double*);
        *value1 = resolveValueRequest.result->doubleValue1;
        *value2 = resolveValueRequest.result->doubleValue2;
        *value3 = resolveValueRequest.result->doubleValue3;
        *value4 = resolveValueRequest.result->doubleValue4;
    } else if (strcmp(paramHandle->type, kOfxParamTypeRGB) == 0) {
        double* value1 = va_arg(ap, double*);
        double* value2 = va_arg(ap, double*);
        double* value3 = va_arg(ap, double*);
        *value1 = resolveValueRequest.result->doubleValue1;
        *value2 = resolveValueRequest.result->doubleValue2;
        *value3 = resolveValueRequest.result->doubleValue3;
    } else if (strcmp(paramHandle->type, kOfxParamTypeChoice) == 0) {
        int* value = va_arg(ap, int*);
        *value = resolveValueRequest.result->intValue1;
        LOG(" returned " << *value );
    } else {
        LOG_ERROR("Type not supported yet " << paramHandle->type );
    }

    delete resolveValueRequest.result;

    return kOfxStatOK;
#endif
}

OfxStatus paramGetDerivative(OfxParamHandle  paramHandle, OfxTime time, ...) {
    LOG_ERROR("paramGetDerivative" );

    return kOfxStatOK;
}

OfxStatus paramGetIntegral(OfxParamHandle  paramHandle, OfxTime time1, OfxTime time2, ...) {
    LOG_ERROR("paramGetIntegral" );

    return kOfxStatOK;
}

OfxStatus paramSetValue(OfxParamHandle  paramHandle, ...) {
    LOG_ERROR("paramSetValue" );

    return kOfxStatOK;
}

OfxStatus paramSetValueAtTime(OfxParamHandle  paramHandle, OfxTime time, ...) {
    LOG_ERROR("paramSetValueAtTime" );

    return kOfxStatOK;
}

OfxStatus paramGetNumKeys(OfxParamHandle  paramHandle, unsigned int  *numberOfKeys) {
    LOG_ERROR("paramGetNumKeys" );

    return kOfxStatOK;
}

OfxStatus paramGetKeyTime(OfxParamHandle  paramHandle, unsigned int nthKey, OfxTime *time) {
    LOG_ERROR("paramGetKeyTime" );

    return kOfxStatOK;
}

OfxStatus paramGetKeyIndex(OfxParamHandle  paramHandle,
            OfxTime time,
            int     direction,
            int    *index) {
                LOG_ERROR("paramGetKeyIndex" );
                return kOfxStatOK;
}

OfxStatus paramDeleteKey(OfxParamHandle  paramHandle, OfxTime time) {
    LOG_ERROR("paramDeleteKey" );

    return kOfxStatOK;
}

OfxStatus paramDeleteAllKeys(OfxParamHandle  paramHandle) {
    LOG_ERROR("paramDeleteAllKeys" );

    return kOfxStatOK;
}

OfxStatus paramCopy(OfxParamHandle  paramTo, OfxParamHandle  paramFrom, OfxTime dstOffset, const OfxRangeD *frameRange) {
    LOG_ERROR("paramCopy" );

    return kOfxStatOK;
}

OfxStatus paramEditBegin(OfxParamSetHandle paramSet, const char *name) {
    LOG_ERROR("paramEditBegin" );

    return kOfxStatOK;
}

OfxStatus paramEditEnd(OfxParamSetHandle paramSet) {
    LOG_ERROR("paramEditEnd" );

    return kOfxStatOK;
}

OfxParameterSuiteV1* getParameterSuite() {
    if (parameterSuite != NULL) {
        return parameterSuite;
    } else {
        parameterSuite = new OfxParameterSuiteV1();
        parameterSuite->paramDefine = &paramDefine;
        parameterSuite->paramSetGetPropertySet = &paramSetGetPropertySet;
        parameterSuite->paramGetPropertySet = &paramGetPropertySet;
        parameterSuite->paramGetValue = &paramGetValue;
        parameterSuite->paramGetValueAtTime = &paramGetValueAtTime;
        parameterSuite->paramGetDerivative = &paramGetDerivative;
        parameterSuite->paramGetIntegral = &paramGetIntegral;
        parameterSuite->paramSetValue = &paramSetValue;
        parameterSuite->paramSetValueAtTime = &paramSetValueAtTime;
        parameterSuite->paramGetNumKeys = &paramGetNumKeys;
        parameterSuite->paramGetKeyTime = &paramGetKeyTime;
        parameterSuite->paramGetKeyIndex = &paramGetKeyIndex;
        parameterSuite->paramDeleteKey = &paramDeleteKey;
        parameterSuite->paramDeleteAllKeys = &paramDeleteAllKeys;
        parameterSuite->paramCopy = &paramCopy;
        parameterSuite->paramEditBegin = &paramEditBegin;
        parameterSuite->paramEditEnd = &paramEditEnd;
        parameterSuite->paramGetHandle = &paramGetHandle;
        return parameterSuite;
    }
}