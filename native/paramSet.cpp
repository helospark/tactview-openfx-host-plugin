#include "paramSet.h"
#include <cstring>
#include <cstdarg>
#include "ofx_property.h"
#include "globalCallbackFunctions.h"
#include "imageEffectStruct.h"

OfxParameterSuiteV1* parameterSuite = NULL;

OfxParamStruct::OfxParamStruct(const char* name, const char* type) {
    this->name = name;
    this->type = type;
    this->properties = new OfxPropertySetStruct();
}

char* copyString(const char* str) {
    int len = strlen(str);
    char* newPointer = new char[len];
    strcpy(newPointer, str);
    return newPointer;
}

OfxStatus paramDefine(OfxParamSetHandle paramSet,
            const char *paramType,
            const char *name,
            OfxPropertySetHandle *propertySet) {
                std::cout << "paramDefine " << name << std::endl;

                OfxParamHandle result = new OfxParamStruct(copyString(name), copyString(paramType)); 

                propSetString(result->properties, kOfxParamPropType, 0, copyString(paramType));
                propSetString(result->properties, kOfxPropName, 0, copyString(name));
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
                    std::cout << "paramGetHandle " << name << " " << param << std::endl;
                    for (int i = 0; i < paramSet->parameters.size(); ++i) {
                        if (strcmp(paramSet->parameters[i]->name, name) == 0) {
                            *param = paramSet->parameters[i];
                            if (propertySet != NULL) {
                                *propertySet = paramSet->parameters[i]->properties;
                            }
                            return kOfxStatOK;
                        }
                    }
                    std::cout << "[!Error!] - " << name  << " not found" << std::endl;
                    for (int i = 0; i < paramSet->parameters.size(); ++i) {
                        std::cout << "   -  " <<  paramSet->parameters[i]->name << std::endl;
                    }
                    return kOfxStatErrBadHandle;
                }

OfxStatus paramSetGetPropertySet(OfxParamSetHandle paramSet, OfxPropertySetHandle *propHandle) {
        // WHUT IS THIS??
    *propHandle = NULL;

    std::cout << "[!ERROR!] paramSetGetPropertySet" << std::endl;
    return kOfxStatOK;

}

OfxStatus paramGetPropertySet(OfxParamHandle param, OfxPropertySetHandle *propHandle) {
    std::cout << "paramGetPropertySet" << std::endl;
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
    std::cout << "paramGetValue " << paramHandle->type << " " << paramHandle->name << std::endl;
    
    va_list ap;
    va_start(ap, paramHandle);

    if (strcmp(paramHandle->name, kOfxImageEffectTransitionParamName) == 0) {
        double* value = va_arg(ap, double*);
        *value = paramHandle->imageEffectHandle->currentRenderRequest->transitionProgress;
        std::cout << "TransitionProgress " << *value << std::endl;
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
        std::cout << " returned " << *value << std::endl;
    } else if (strcmp(paramHandle->type, kOfxParamTypeDouble) == 0) {
        double* value = va_arg(ap, double*);
        *value = resolveValueRequest.result->doubleValue1;
        std::cout << " returned " << *value << std::endl;
    } else if (strcmp(paramHandle->type, kOfxParamTypeBoolean) == 0) {
        int* value = va_arg(ap, int*);
        *value = resolveValueRequest.result->intValue1;
        std::cout << " returned " << *value << std::endl;
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
        std::cout << "Returned " << *value1 << " " << *value2 << std::endl;
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
        std::cout << " returned " << *value << std::endl;
    } else {
        std::cout << "[!ERROR!] Type not supported yet " << paramHandle->type << std::endl;
    }
    delete resolveValueRequest.result;

    return kOfxStatOK;
}

OfxStatus paramGetValueAtTime(OfxParamHandle  paramHandle, OfxTime time, ...) {
    std::cout << "paramGetValueAtTime " << paramHandle->name << std::endl;
    va_list ap;
    va_start(ap, time);

    if (strcmp(paramHandle->name, kOfxImageEffectTransitionParamName) == 0) {
        double* value = va_arg(ap, double*);
        *value = paramHandle->imageEffectHandle->currentRenderRequest->transitionProgress;
        std::cout << "TransitionProgress " << *value << std::endl;
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
        std::cout << " returned " << *value << std::endl;
    } else if (strcmp(paramHandle->type, kOfxParamTypeDouble) == 0) {
        double* value = va_arg(ap, double*);
        *value = resolveValueRequest.result->doubleValue1;
        std::cout << " returned " << *value << std::endl;
    } else if (strcmp(paramHandle->type, kOfxParamTypeBoolean) == 0) {
        int* value = va_arg(ap, int*);
        *value = resolveValueRequest.result->intValue1;
        std::cout << " returned " << *value << std::endl;
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

        std::cout << "returned " << *value1 << " " << *value2 << std::endl;
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
        std::cout << " returned " << *value << std::endl;
    } else {
        std::cout << "[!ERROR!] Type not supported yet " << paramHandle->type << std::endl;
    }

    delete resolveValueRequest.result;

    return kOfxStatOK;
}

OfxStatus paramGetDerivative(OfxParamHandle  paramHandle, OfxTime time, ...) {
    std::cout << "[!ERROR!]  paramGetDerivative" << std::endl;

    return kOfxStatOK;
}

OfxStatus paramGetIntegral(OfxParamHandle  paramHandle, OfxTime time1, OfxTime time2, ...) {
    std::cout << "[!ERROR!] paramGetIntegral" << std::endl;

    return kOfxStatOK;
}

OfxStatus paramSetValue(OfxParamHandle  paramHandle, ...) {
    std::cout << "[!ERROR!] paramSetValue" << std::endl;

    return kOfxStatOK;
}

OfxStatus paramSetValueAtTime(OfxParamHandle  paramHandle, OfxTime time, ...) {
    std::cout << "[!ERROR!] paramSetValueAtTime" << std::endl;

    return kOfxStatOK;
}

OfxStatus paramGetNumKeys(OfxParamHandle  paramHandle, unsigned int  *numberOfKeys) {
    std::cout << "[!ERROR!] paramGetNumKeys" << std::endl;

    return kOfxStatOK;
}

OfxStatus paramGetKeyTime(OfxParamHandle  paramHandle, unsigned int nthKey, OfxTime *time) {
    std::cout << "[!ERROR!] paramGetKeyTime" << std::endl;

    return kOfxStatOK;
}

OfxStatus paramGetKeyIndex(OfxParamHandle  paramHandle,
            OfxTime time,
            int     direction,
            int    *index) {
                std::cout << "[!ERROR!] paramGetKeyIndex" << std::endl;
                return kOfxStatOK;
            }

OfxStatus paramDeleteKey(OfxParamHandle  paramHandle, OfxTime time) {
    std::cout << "[!ERROR!] paramDeleteKey" << std::endl;

    return kOfxStatOK;
}

OfxStatus paramDeleteAllKeys(OfxParamHandle  paramHandle) {
    std::cout << "[!ERROR!] paramDeleteAllKeys" << std::endl;

    return kOfxStatOK;
}

OfxStatus paramCopy(OfxParamHandle  paramTo, OfxParamHandle  paramFrom, OfxTime dstOffset, const OfxRangeD *frameRange) {
    std::cout << "[!ERROR!] paramCopy" << std::endl;

    return kOfxStatOK;
}

OfxStatus paramEditBegin(OfxParamSetHandle paramSet, const char *name) {
    std::cout << "[!ERROR!] paramEditBegin" << std::endl;

    return kOfxStatOK;
}

OfxStatus paramEditEnd(OfxParamSetHandle paramSet) {
    std::cout << "[!ERROR!] paramEditEnd" << std::endl;

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