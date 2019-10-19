#include "messageSuite.h"
#include <cstdio>
#include <iostream>
#include "global.h"

OfxMessageSuiteV2* messageSuite = NULL;

OfxStatus message(void *handle,
            const char *messageType,
            const char *messageId,
            const char *format,
            ...) {
                std::cout <<  "[MESSAGE] " << messageId << " " << format << std::endl;
                return kOfxStatOK;
}

OfxStatus clearPersistentMessage(void *handle) {
    LOG("clearPersistentMessage" );
    return kOfxStatOK;
}

OfxStatus setPersistentMessage(void *handle,
                                const char *messageType,
                                const char *messageId,
                                const char *format,
                                ...) {
    std::cout << "[PERSISTENT_MESSAGE] " <<  messageId << " " << format << std::endl;
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