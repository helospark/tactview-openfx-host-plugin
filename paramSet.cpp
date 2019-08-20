#include "paramSet.h"
#include <cstring>
#include <cstdarg>
#include "ofx_property.h"

OfxParameterSuiteV1* parameterSuite = NULL;

OfxParamStruct::OfxParamStruct(const char* name, const char* type) {
    this->name = name;
    this->type = type;
    this->properties = new OfxPropertySetStruct();
}

OfxStatus paramDefine(OfxParamSetHandle paramSet,
            const char *paramType,
            const char *name,
            OfxPropertySetHandle *propertySet) {
                std::cout << "paramDefine " << name << std::endl;

                OfxParamHandle result = new OfxParamStruct(name, paramType); 

                propSetString(result->properties, kOfxParamPropType, 0, paramType);
                propSetString(result->properties, kOfxPropName, 0, name);

                paramSet->parameters.push_back(result);

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

OfxStatus paramGetValue(OfxParamHandle  paramHandle, ...) {
    std::cout << "paramGetValue " << paramHandle->type << " " << paramHandle->name << std::endl;
    
    va_list ap;
    va_start(ap, paramHandle);

    if (strcmp(paramHandle->type, kOfxParamTypeInteger) == 0) {
        int* value = va_arg(ap, int*);
        *value = paramHandle->properties->integers[kOfxParamPropDefault][0];
        std::cout << " returned " << *value << std::endl;
    } else if (strcmp(paramHandle->type, kOfxParamTypeDouble) == 0) {
        double* value = va_arg(ap, double*);
        *value = paramHandle->properties->doubles[kOfxParamPropDefault][0];
        std::cout << " returned " << *value << std::endl;
    } else if (strcmp(paramHandle->type, kOfxParamTypeBoolean) == 0) {
        int* value = va_arg(ap, int*);
        *value = paramHandle->properties->integers[kOfxParamPropDefault][0];
        //*value = 1;
        std::cout << " returned " << *value << std::endl;
    } else if (strcmp(paramHandle->type, kOfxParamTypeInteger2D)== 0) {
        int* value = va_arg(ap, int*);
        *value = paramHandle->properties->integers[kOfxParamPropDefault][0];
        int* value2 = va_arg(ap, int*);
        *value2 = paramHandle->properties->integers[kOfxParamPropDefault][1];
        std::cout << "Integer2D returned " << *value << " " << *value2 << std::endl;
    } else if (strcmp(paramHandle->type, kOfxParamTypeDouble2D) == 0) {
        int* value = va_arg(ap, int*);
        *value = paramHandle->properties->doubles[kOfxParamPropDefault][0];
        int* value2 = va_arg(ap, int*);
        *value2 = paramHandle->properties->doubles[kOfxParamPropDefault][1];
        std::cout << "Double2D returned " << *value << " " << *value2 << std::endl;
    } else if (strcmp(paramHandle->type, kOfxParamTypeRGBA) == 0) {
        for (int i = 0; i < 4; ++i) {
            int* value = va_arg(ap, int*);
            *value = paramHandle->properties->doubles[kOfxParamPropDefault][i] * 255.0;
            std::cout << "RGBA[" << i << "] = " << *value << std::endl;
        }
    } else if (strcmp(paramHandle->type, kOfxParamTypeChoice) == 0) {
        int* value = va_arg(ap, int*);
        auto val = paramHandle->properties->integers[kOfxParamPropDefault];
        if (val.size() > 0) {
            *value = val[0];
        } else {
            *value = 0;
        }
        std::cout << " returned " << *value << std::endl;
    } else {
        std::cout << "[!ERROR!] Type not supported yet " << paramHandle->type << std::endl;
    }

    return kOfxStatOK;
}

OfxStatus paramGetValueAtTime(OfxParamHandle  paramHandle, OfxTime time, ...) {
    std::cout << "paramGetValueAtTime " << paramHandle->name << std::endl;
    va_list ap;
    va_start(ap, time);

    if (strcmp(paramHandle->type, kOfxParamTypeInteger) == 0) {
        int* value = va_arg(ap, int*);
        *value = paramHandle->properties->integers[kOfxParamPropDefault][0];
    } else if (strcmp(paramHandle->type, kOfxParamTypeDouble) == 0) {
        double* value = va_arg(ap, double*);
        *value = 0.5;
    } else if (strcmp(paramHandle->type, kOfxParamTypeBoolean) == 0) {
        int* value = va_arg(ap, int*);
        *value = 0;
    } else {
        std::cout << "[!ERROR!] Type not supported yet " << paramHandle->type << std::endl;
    }
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