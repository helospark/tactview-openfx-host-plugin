#include "string_operations.h"
#include <cstring>

char* duplicateString(const char* str) {
    int len = strlen(str);
    char* result = new char[len + 1];
    strcpy(result, str);
    result[len] = '\0';

    return result;
}