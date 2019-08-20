#include "messageSuite.h"
#include <cstdio>
#include <iostream>

OfxMessageSuiteV2* messageSuite = NULL;

OfxStatus message(void *handle,
            const char *messageType,
            const char *messageId,
            const char *format,
            ...) {
                fprintf(stdout, "[MESSAGE] %s %s", messageId, format);
                return kOfxStatOK;
}

OfxStatus clearPersistentMessage(void *handle) {
    std::cout << "clearPersistentMessage" << std::endl;
    return kOfxStatOK;
}

OfxStatus setPersistentMessage(void *handle,
                                const char *messageType,
                                const char *messageId,
                                const char *format,
                                ...) {
    fprintf(stdout, "[PERSISTENT_MESSAGE] %s %s", messageId, format);
    return kOfxStatOK;
}

OfxMessageSuiteV2* getMessageSuite() {
    if (messageSuite != NULL) {
        return messageSuite;
    } else {
        messageSuite = new OfxMessageSuiteV2();
        messageSuite->message = &message;
        messageSuite->setPersistentMessage = &setPersistentMessage;
        messageSuite->clearPersistentMessage = &clearPersistentMessage;
        return messageSuite;
    }
}