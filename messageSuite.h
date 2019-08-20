#include "openfx/include/ofxMessage.h"

OfxStatus message(void *handle,
            const char *messageType,
            const char *messageId,
            const char *format,
            ...);
OfxStatus clearPersistentMessage(void *handle);
OfxStatus setPersistentMessage(void *handle,
                                const char *messageType,
                                const char *messageId,
                                const char *format,
                                ...);

OfxMessageSuiteV2* getMessageSuite();