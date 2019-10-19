#include <cstring>
#include <iostream>
#include "ofx_property.h"
#include "string_operations.h"
#include <sstream>
#include "global.h"

OfxPropertySuiteV1* propertySuiteV1 = NULL;

OfxStatus propGetDoubleN(OfxPropertySetHandle properties, const char *property, int count, double *value) {
    LOG("propGetDoubleN" );

    auto val = properties->doubles[std::string(property)];

    if (val.size() < count) {
        LOG_ERROR("No handle " << property );
        return kOfxStatErrBadIndex;
    }

    for (int i = 0; i < count; ++i) {
        value[i] = val[i];
    }

    return kOfxStatOK;
}

OfxStatus propGetIntN(OfxPropertySetHandle properties, const char *property, int count, int *value) {
    LOG("propGetIntN " << property );

    auto val = properties->integers[std::string(property)];

    if (val.size() < count) {
        LOG("NOT ENOUGH values " << val.size() << " " << count );
        printAllProperties(properties);
        return kOfxStatErrBadIndex;
    }

    for (int i = 0; i < count; ++i) {
        LOG("propGetIntN[] " << val[i] );
        value[i] = val[i];
    }

    return kOfxStatOK;
}

OfxStatus propReset(OfxPropertySetHandle properties, const char *property) {
    LOG("propReset" );
    properties->doubles.erase(std::string(property));
    properties->integers.erase(std::string(property));
    properties->pointers.erase(std::string(property));

    auto it = properties->strings.find(std::string(property));
    for (auto e : it->second) {
        delete[] e;
    }
    properties->strings.erase(it);
    return kOfxStatOK;
}

OfxStatus propGetDimension(OfxPropertySetHandle properties, const char *property, int *count) {
    LOG("propGetDimension " << property );
    std::map<std::string,std::vector<double>>::iterator doubleIterator = properties->doubles.find(std::string(property));
    if (doubleIterator != properties->doubles.end()) {
        *count = (*doubleIterator).second.size();
        return kOfxStatOK;
    }

    std::map<std::string,std::vector<int>>::iterator intIterator = properties->integers.find(std::string(property));
    if (intIterator != properties->integers.end()) {
        *count = (*intIterator).second.size();
        return kOfxStatOK;
    }

    std::map<std::string,std::vector<void*>>::iterator pointerIterator = properties->pointers.find(std::string(property));
    if (pointerIterator != properties->pointers.end()) {
        *count = (*pointerIterator).second.size();
        return kOfxStatOK;
    }

    std::map<std::string,std::vector<char*>>::iterator stringIterator = properties->strings.find(std::string(property));
    if (stringIterator != properties->strings.end()) {
        *count = (*stringIterator).second.size();
        return kOfxStatOK;
    }

    *count = 0;

    LOG("       No dimenstion " << property );
    return kOfxStatOK;
}

OfxStatus propSetPointer(OfxPropertySetHandle properties, const char *property, int index, void *value) {
    LOG("propSetPointer " << property << " " << value );
    std::string sproperty(property);
    if (properties->pointers[sproperty].size() <= index) {
        properties->pointers[sproperty].resize(index + 1);
    }
    properties->pointers[sproperty].at(index) = value;
    return kOfxStatOK;
}

OfxStatus propSetString(OfxPropertySetHandle properties, const char *property, int index, const char *value) {
    LOG("propSetString " << property << " " << value );
    std::string sproperty(property);
    if (properties->strings[sproperty].size() <= index) {
        properties->strings[sproperty].resize(index + 1);
    }
    properties->strings[sproperty].at(index) = duplicateString(value);
    LOG("Successfully appended" );
    return kOfxStatOK;
}

OfxStatus propSetDouble(OfxPropertySetHandle properties, const char *property, int index, double value) {
    LOG("propSetDouble " << property );
    std::string sproperty(property);
    if (properties->doubles[sproperty].size() <= index) {
        properties->doubles[sproperty].resize(index + 1);
    }
    properties->doubles[sproperty].at(index) = value;
    return kOfxStatOK;
}

OfxStatus propSetInt(OfxPropertySetHandle properties, const char *property, int index, int value) {
    LOG("propSetInt " << property << " " << index << " " << value );
    std::string sproperty(property);
    LOG("Setting " << sproperty << " " << properties->integers.size() );
    if (properties->integers[sproperty].size() <= index) {
        LOG("Resizing " << sproperty << " " << (index + 1) );
        properties->integers[sproperty].resize(index + 1);
    }
    properties->integers[sproperty].at(index) = value;

    return kOfxStatOK;
}

OfxStatus propSetPointerN(OfxPropertySetHandle properties, const char *property, int count, void *const*value) {
    LOG("propSetPointerN" );
    std::string sproperty(property);

    properties->pointers[sproperty].resize(count);

    for (int i = 0; i < count; ++i) {
        properties->pointers[sproperty].at(i) = value[i];
    }
    return kOfxStatOK;
}

OfxStatus propSetStringN(OfxPropertySetHandle properties, const char *property, int count, const char *const*value) {
    
    LOG("propSetStringN" );
    std::string sproperty(property);

    properties->strings[sproperty].resize(count);

    for (int i = 0; i < count; ++i) {
        properties->strings[sproperty].at(i) = duplicateString(value[i]);
    }
    return kOfxStatOK;
}

OfxStatus propSetDoubleN(OfxPropertySetHandle properties, const char *property, int count, const double *value) {
    std::string sproperty(property);

    properties->doubles[sproperty].resize(count);

    for (int i = 0; i < count; ++i) {
        properties->doubles[sproperty].at(i) = value[i];
    }
    return kOfxStatOK;
}

OfxStatus propSetIntN(OfxPropertySetHandle properties, const char *property, int count, const int *value) {
    LOG("propSetIntN" );
    std::string sproperty(property);

    properties->integers[sproperty].resize(count);

    for (int i = 0; i < count; ++i) {
        LOG("propSetIntN[] " << value[i]);
        properties->integers[sproperty].at(i) = value[i];
    }
    return kOfxStatOK;
}

OfxStatus propGetPointer(OfxPropertySetHandle properties, const char *property, int index, void **value) {
    LOG("propGetPointer " << property );
    std::string sproperty(property);
    std::map<std::string,std::vector<void*>>::iterator pointerIterator = properties->pointers.find(std::string(property));

    if (pointerIterator == properties->pointers.end()) {
        LOG_ERROR("No handle " << property );
        return kOfxStatErrBadHandle;
    }

    std::vector<void*> result = (*pointerIterator).second;

    if (index < result.size()) {
        *value = result.at(index);
        LOG("    Returning " << *value );
        return kOfxStatOK;
    } else {
        LOG_ERROR("No handle " << property );
        return kOfxStatErrBadIndex;
    }
}

OfxStatus propGetString(OfxPropertySetHandle properties, const char *property, int index, char **value) {
    LOG("propGetString " << property );
    std::string sproperty(property);
    std::map<std::string,std::vector<char*>>::iterator pointerIterator = properties->strings.find(std::string(property));

    if (pointerIterator == properties->strings.end()) {
        LOG_ERROR("No handle " << property );
        printAllProperties(properties);
        return kOfxStatErrBadHandle;
    }

    std::vector<char*> result = (*pointerIterator).second;

    if (index < result.size()) {
        *value = result.at(index);
        LOG("Success, returning " << *value );
        return kOfxStatOK;
    } else {
        LOG_ERROR("No handle " << property );
        printAllProperties(properties);
        return kOfxStatErrBadIndex;
    }
}

OfxStatus propGetDouble(OfxPropertySetHandle properties, const char *property, int index, double *value) {
    LOG("propGetDouble " << property << " " << index );
    std::string sproperty(property);
    std::map<std::string,std::vector<double>>::iterator doubleIterator = properties->doubles.find(std::string(property));

    if (doubleIterator == properties->doubles.end()) {
        LOG_ERROR("No handle " << property );
        printAllProperties(properties);
        return kOfxStatErrBadHandle;
    }

    std::vector<double> result = (*doubleIterator).second;

    if (index < result.size()) {
        *value = result.at(index);
        LOG("        returning " << *value );
        return kOfxStatOK;
    } else {
        printAllProperties(properties);
        LOG("No handle " << property );
        return kOfxStatErrBadIndex;
    }
}

OfxStatus propGetInt(OfxPropertySetHandle properties, const char *property, int index, int *value) {
    LOG("propGetInt " << property << " " << index );
    std::string sproperty(property);
    std::map<std::string,std::vector<int>>::iterator doubleIterator = properties->integers.find(std::string(property));

    if (doubleIterator == properties->integers.end()) {
        LOG("[!ERROR!] - No handle " << property );
        return kOfxStatErrBadHandle;
    }

    std::vector<int> result = (*doubleIterator).second;

    if (index < result.size()) {
        *value = result.at(index);
        LOG("         returning " << *value );
        return kOfxStatOK;
    } else {
        LOG_ERROR("No handle " << property );
        return kOfxStatErrBadIndex;
    }
}

OfxStatus propGetPointerN(OfxPropertySetHandle properties, const char *property, int count, void **value) {
    LOG("" );
    std::string sproperty(property);
    std::map<std::string,std::vector<void*>>::iterator doubleIterator = properties->pointers.find(std::string(property));

    if (doubleIterator == properties->pointers.end()) {
        LOG_ERROR(" No handle " << property );
        return kOfxStatErrBadHandle;
    }

    std::vector<void*> result = (*doubleIterator).second;
    if (count >= result.size()) {
        return kOfxStatErrBadIndex;
    } else {
        for (int i = 0; i <count; ++i) {
            value[i] = result[i];
        }
    }
    return kOfxStatOK;
}

OfxStatus propGetStringN(OfxPropertySetHandle properties, const char *property, int count, char **value) {
    LOG("" );
    std::string sproperty(property);
    std::map<std::string,std::vector<char*>>::iterator doubleIterator = properties->strings.find(std::string(property));

    if (doubleIterator == properties->strings.end()) {
        LOG("[!ERROR!] - No handle " << property );
        return kOfxStatErrBadHandle;
    }

    std::vector<char*> result = (*doubleIterator).second;
    if (count >= result.size()) {
        LOG_ERROR("No handle " << property );
       return kOfxStatErrBadIndex;
    } else {
        for (int i = 0; i <count; ++i) {
            value[i] = result[i];
        }
    }
    return kOfxStatOK;
}

OfxPropertySuiteV1* createPropertySuiteV1() {
    if (propertySuiteV1 != NULL) {
        return propertySuiteV1;
    } else {
        propertySuiteV1 = new OfxPropertySuiteV1();
        propertySuiteV1->propGetDoubleN=&propGetDoubleN;
        propertySuiteV1->propGetIntN=&propGetIntN;
        propertySuiteV1->propReset=&propReset;
        propertySuiteV1->propGetDimension=&propGetDimension;
        propertySuiteV1->propSetPointer=&propSetPointer;
        propertySuiteV1->propSetString=&propSetString;
        propertySuiteV1->propSetDouble=&propSetDouble;
        propertySuiteV1->propSetInt=&propSetInt;
        propertySuiteV1->propSetPointerN=&propSetPointerN;
        propertySuiteV1->propSetStringN=&propSetStringN;
        propertySuiteV1->propSetDoubleN=&propSetDoubleN;
        propertySuiteV1->propSetIntN=&propSetIntN;
        propertySuiteV1->propGetPointer=&propGetPointer;
        propertySuiteV1->propGetString=&propGetString;
        propertySuiteV1->propGetDouble=&propGetDouble;
        propertySuiteV1->propGetInt=&propGetInt;
        propertySuiteV1->propGetPointerN=&propGetPointerN;
        propertySuiteV1->propGetStringN=&propGetStringN;

        return propertySuiteV1;
    }
}

template<class T>
void printMap(std::map<std::string, std::vector<T>> map) {
    #ifdef DEBUG_LOGGING
        for (auto& t : map) {
            std::cout << t.first << "  :   ";
            for (int i = 0; i < t.second.size(); ++i) {
                std::cout << t.second[i] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    #endif
}

void printAllProperties(OfxPropertySetHandle properties) {
    #ifdef DEBUG_LOGGING
        std::cout << "Values" << std::endl;
        printMap(properties->doubles );
        printMap(properties->integers);
        printMap(properties->pointers);
        printMap(properties->strings);
        std::cout << "//Values" << std::endl;
    #endif
}

std::map<std::string, std::vector<char*>>* getParametersAsMap(OfxPropertySetHandle properties) {
    std::map<std::string, std::vector<char*>>* resultPointer = new std::map<std::string, std::vector<char*>>();

    std::map<std::string, std::vector<char*>>& result = *resultPointer;

    for (auto& t : properties->strings) {
        std::vector<char*> elementLine;

        for (auto e : t.second) {
            elementLine.push_back(duplicateString(e));
        }

        result[t.first] = elementLine;
    }
    for (auto& t : properties->integers) {
        std::vector<char*> elementLine;

        for (auto e : t.second) {
            std::string out_string;
            std::stringstream ss;
            ss << e;
            out_string = ss.str();

            elementLine.push_back(duplicateString(ss.str().c_str()));
        }

        result[t.first] = elementLine;
    }
    for (auto& t : properties->doubles) {
        std::vector<char*> elementLine;

        for (auto e : t.second) {
            std::string out_string;
            std::stringstream ss;
            ss << e;
            out_string = ss.str();

            elementLine.push_back(duplicateString(ss.str().c_str()));
        }

        result[t.first] = elementLine;
    }
    for (auto& t : properties->pointers) {
        LOG("ERROR pointer cannot be serialized" );
    }

    return resultPointer;
}

OfxPropertySetHandle copyProperties(OfxPropertySetHandle from) {
    OfxPropertySetHandle result = new OfxPropertySetStruct();
    for (auto element : from->strings) {
        std::vector<char*> list;
        for (auto a : element.second) {
            list.push_back(duplicateString(a));
        }
        result->strings[element.first] = list;
    }
    for (auto element : from->integers) {
        std::vector<int> list;
        for (auto a : element.second) {
            list.push_back(a);
        }
        result->integers[element.first] = list;
    }
    for (auto element : from->doubles) {
        std::vector<double> list;
        for (auto a : element.second) {
            list.push_back(a);
        }
        result->doubles[element.first] = list;
    }
    for (auto element : from->pointers) {
        std::vector<void*> list;
        for (auto a : element.second) {
            list.push_back(a);
        }
        result->pointers[element.first] = list;
    }
    return result;
}

