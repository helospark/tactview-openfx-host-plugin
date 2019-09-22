#include <cstring>
#include <iostream>
#include "ofx_property.h"
#include "string_operations.h"
#include <sstream>

OfxPropertySuiteV1* propertySuiteV1 = NULL;

char* copyString(OfxPropertySetHandle property, const char* str) {
    int len = strlen(str);
    char* newPointer = new char[len + 1];
    strcpy(newPointer, str);
    property->stringToClean.push_back(newPointer);
    return newPointer;
}

OfxStatus propGetDoubleN(OfxPropertySetHandle properties, const char *property, int count, double *value) {
    std::cout << "propGetDoubleN" << std::endl;

    auto val = properties->doubles[std::string(property)];

    if (val.size() < count) {
        std::cout << "[!ERROR!] - No handle " << property << std::endl;
        return kOfxStatErrBadIndex;
    }

    for (int i = 0; i < count; ++i) {
        value[i] = val[i];
    }

    return kOfxStatOK;
}

OfxStatus propGetIntN(OfxPropertySetHandle properties, const char *property, int count, int *value) {
    std::cout << "propGetIntN " << property << std::endl;

    auto val = properties->integers[std::string(property)];

    if (val.size() < count) {
        std::cout << "NOT ENOUGH values " << val.size() << " " << count << std::endl;
        printAllProperties(properties);
        return kOfxStatErrBadIndex;
    }

    for (int i = 0; i < count; ++i) {
        std::cout << "propGetIntN[] " << val[i] << std::endl;
        value[i] = val[i];
    }

    return kOfxStatOK;
}

OfxStatus propReset(OfxPropertySetHandle properties, const char *property) {
    std::cout << "propReset" << std::endl;
    properties->doubles.erase(std::string(property));
    properties->integers.erase(std::string(property));
    properties->pointers.erase(std::string(property));
    properties->strings.erase(std::string(property));
    return kOfxStatOK;
}

OfxStatus propGetDimension(OfxPropertySetHandle properties, const char *property, int *count) {
    std::cout << "propGetDimension " << property << std::endl;
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

    std::cout << "       No dimenstion " << property << std::endl;
    return kOfxStatOK;
}

OfxStatus propSetPointer(OfxPropertySetHandle properties, const char *property, int index, void *value) {
    std::cout << "propSetPointer " << property << " " << value << std::endl;
    std::string sproperty(property);
    if (properties->pointers[sproperty].size() <= index) {
        properties->pointers[sproperty].resize(index + 1);
    }
    properties->pointers[sproperty].at(index) = value;
    return kOfxStatOK;
}

OfxStatus propSetString(OfxPropertySetHandle properties, const char *property, int index, const char *value) {
    std::cout << "propSetString " << property << " " << value << std::endl;
    std::string sproperty(property);
    if (properties->strings[sproperty].size() <= index) {
        properties->strings[sproperty].resize(index + 1);
    }
    properties->strings[sproperty].at(index) = copyString(properties, value);
    std::cout << "Successfully appended" << std::endl;
    return kOfxStatOK;
}

OfxStatus propSetDouble(OfxPropertySetHandle properties, const char *property, int index, double value) {
    std::cout << "propSetDouble " << property << std::endl;
    std::string sproperty(property);
    if (properties->doubles[sproperty].size() <= index) {
        properties->doubles[sproperty].resize(index + 1);
    }
    properties->doubles[sproperty].at(index) = value;
    return kOfxStatOK;
}

OfxStatus propSetInt(OfxPropertySetHandle properties, const char *property, int index, int value) {
    std::cout << "propSetInt " << property << " " << index << " " << value << std::endl;
    std::string sproperty(property);
    std::cout << "Setting " << sproperty << " " << properties->integers.size() << std::endl;
    if (properties->integers[sproperty].size() <= index) {
        std::cout << "Resizing " << sproperty << " " << (index + 1) << std::endl;
        properties->integers[sproperty].resize(index + 1);
    }
    properties->integers[sproperty].at(index) = value;

    return kOfxStatOK;
}

OfxStatus propSetPointerN(OfxPropertySetHandle properties, const char *property, int count, void *const*value) {
    std::cout << "propSetPointerN" << std::endl;
    std::string sproperty(property);

    properties->pointers[sproperty].resize(count);

    for (int i = 0; i < count; ++i) {
        properties->pointers[sproperty].at(i) = value[i];
    }
    return kOfxStatOK;
}

OfxStatus propSetStringN(OfxPropertySetHandle properties, const char *property, int count, const char *const*value) {
    
    std::cout << "propSetStringN" << std::endl;
    std::string sproperty(property);

    properties->strings[sproperty].resize(count);

    for (int i = 0; i < count; ++i) {
        properties->strings[sproperty].at(i) = copyString(properties, value[i]);
    }
    return kOfxStatOK;
}

OfxStatus propSetDoubleN(OfxPropertySetHandle properties, const char *property, int count, const double *value) {
    std::cout << "" << std::endl;
    std::string sproperty(property);

    properties->doubles[sproperty].resize(count);

    for (int i = 0; i < count; ++i) {
        properties->doubles[sproperty].at(i) = value[i];
    }
    return kOfxStatOK;
}

OfxStatus propSetIntN(OfxPropertySetHandle properties, const char *property, int count, const int *value) {
    std::cout << "propSetIntN" << std::endl;
    std::string sproperty(property);

    properties->integers[sproperty].resize(count);

    for (int i = 0; i < count; ++i) {
        std::cout << "propSetIntN[] " << value[i]<< std::endl;
        properties->integers[sproperty].at(i) = value[i];
    }
    return kOfxStatOK;
}

OfxStatus propGetPointer(OfxPropertySetHandle properties, const char *property, int index, void **value) {
    std::cout << "propGetPointer " << property << std::endl;
    std::string sproperty(property);
    std::map<std::string,std::vector<void*>>::iterator pointerIterator = properties->pointers.find(std::string(property));

    if (pointerIterator == properties->pointers.end()) {
        std::cout << "[!ERROR!] - No handle " << property << std::endl;
        return kOfxStatErrBadHandle;
    }

    std::vector<void*> result = (*pointerIterator).second;

    if (index < result.size()) {
        *value = result.at(index);
        std::cout << "    Returning " << *value << std::endl;
        return kOfxStatOK;
    } else {
        std::cout << "[!ERROR!] - No handle " << property << std::endl;
        return kOfxStatErrBadIndex;
    }
}

OfxStatus propGetString(OfxPropertySetHandle properties, const char *property, int index, char **value) {
    std::cout << "propGetString " << property << std::endl;
    std::string sproperty(property);
    std::map<std::string,std::vector<char*>>::iterator pointerIterator = properties->strings.find(std::string(property));

    if (pointerIterator == properties->strings.end()) {
        std::cout << "[!ERROR!] - No handle " << property << std::endl;
        printAllProperties(properties);
        return kOfxStatErrBadHandle;
    }

    std::vector<char*> result = (*pointerIterator).second;

    if (index < result.size()) {
        *value = result.at(index);
        std::cout << "Success, returning " << *value << std::endl;
        return kOfxStatOK;
    } else {
        std::cout << "[!ERROR!] - No handle " << property << std::endl;
        printAllProperties(properties);
        return kOfxStatErrBadIndex;
    }
}

OfxStatus propGetDouble(OfxPropertySetHandle properties, const char *property, int index, double *value) {
    std::cout << "propGetDouble " << property << " " << index << std::endl;
    std::string sproperty(property);
    std::map<std::string,std::vector<double>>::iterator doubleIterator = properties->doubles.find(std::string(property));

    if (doubleIterator == properties->doubles.end()) {
        std::cout << "[!ERROR!] - No handle " << property << std::endl;
        printAllProperties(properties);
        return kOfxStatErrBadHandle;
    }

    std::vector<double> result = (*doubleIterator).second;

    if (index < result.size()) {
        *value = result.at(index);
        std::cout << "        returning " << *value << std::endl;
        return kOfxStatOK;
    } else {
        printAllProperties(properties);
        std::cout << "[!ERROR!] - No handle " << property << std::endl;
        return kOfxStatErrBadIndex;
    }
}

OfxStatus propGetInt(OfxPropertySetHandle properties, const char *property, int index, int *value) {
    std::cout << "propGetInt " << property << " " << index << std::endl;
    std::string sproperty(property);
    std::map<std::string,std::vector<int>>::iterator doubleIterator = properties->integers.find(std::string(property));

    if (doubleIterator == properties->integers.end()) {
        std::cout << "[!ERROR!] - No handle " << property << std::endl;
        return kOfxStatErrBadHandle;
    }

    std::vector<int> result = (*doubleIterator).second;

    if (index < result.size()) {
        *value = result.at(index);
        std::cout << "         returning " << *value << std::endl;
        return kOfxStatOK;
    } else {
        std::cout << "[!ERROR!] - No handle " << property << std::endl;
        return kOfxStatErrBadIndex;
    }
}

OfxStatus propGetPointerN(OfxPropertySetHandle properties, const char *property, int count, void **value) {
    std::cout << "" << std::endl;
    std::string sproperty(property);
    std::map<std::string,std::vector<void*>>::iterator doubleIterator = properties->pointers.find(std::string(property));

    if (doubleIterator == properties->pointers.end()) {
        std::cout << "[!ERROR!] - No handle " << property << std::endl;
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
    std::cout << "" << std::endl;
    std::string sproperty(property);
    std::map<std::string,std::vector<char*>>::iterator doubleIterator = properties->strings.find(std::string(property));

    if (doubleIterator == properties->strings.end()) {
        std::cout << "[!ERROR!] - No handle " << property << std::endl;
        return kOfxStatErrBadHandle;
    }

    std::vector<char*> result = (*doubleIterator).second;
    if (count >= result.size()) {
        std::cout << "[!ERROR!] - No handle " << property << std::endl;
       return kOfxStatErrBadIndex;
    } else {
        for (int i = 0; i <count; ++i) {
            value[i] = copyString(properties, result[i]);
        }
    }
    return kOfxStatOK;
}

OfxPropertySuiteV1* createPropertySuiteV1() {
    std::cout << "" << std::endl;
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
    for (auto& t : map) {
        std::cout << t.first << "  :   ";
        for (int i = 0; i < t.second.size(); ++i) {
            std::cout << t.second[i] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void printAllProperties(OfxPropertySetHandle properties) {
    std::cout << "Values" << std::endl;
    printMap(properties->doubles );
    printMap(properties->integers);
    printMap(properties->pointers);
    printMap(properties->strings);
    std::cout << "//Values" << std::endl;
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
        std::cout << "ERROR pointer cannot be serialized" << std::endl;
    }

    return resultPointer;
}


