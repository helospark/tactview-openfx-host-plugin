#include "imageEffectStruct.h"
#include <iostream>
#include "paramSet.h"
#include "imageLoader.h"
#include <cstring>

OfxImageEffectSuiteV1* effectSuite = NULL;

OfxImageEffectStruct::OfxImageEffectStruct() {
    properties = new OfxPropertySetStruct();
    parameters = new OfxParamSetStruct();
}

OfxStatus getPropertySet(OfxImageEffectHandle imageEffect, OfxPropertySetHandle *propHandle) {
    std::cout << "getPropertySet" << std::endl;
    *propHandle = imageEffect->properties;
    return kOfxStatOK;
}

OfxStatus getParamSet(OfxImageEffectHandle imageEffect, OfxParamSetHandle *paramSet){
    std::cout << "getParamSet" << std::endl;

    *paramSet = imageEffect->parameters;

    return kOfxStatOK;
}
OfxStatus clipDefine(OfxImageEffectHandle imageEffect,
            const char *name,	 
            OfxPropertySetHandle *propertySet) {
                std::cout << "clipDefine " << name << std::endl;


                OfxPropertySetHandle clip = new OfxPropertySetStruct();
                propSetString(clip, "CLIP_NAME", 0, name);
                const char* supportedComponents[] = {"kOfxImageComponentRGBA", "kOfxImageComponentAlpha"};
                propSetStringN(clip, kOfxImageEffectPropSupportedComponents, 2, supportedComponents);
                propSetString(clip, kOfxImageEffectPropPixelDepth, 0, kOfxBitDepthByte);
                propSetString(clip, kOfxImageEffectPropComponents, 0, kOfxImageComponentRGBA);
                propSetInt(clip, kOfxImageClipPropConnected, 0, 1);


                OfxImageClipStruct* clipStruct = new OfxImageClipStruct();
                clipStruct->properties = clip;

                imageEffect->clips[std::string(name)] = clipStruct;

                *propertySet = clip;

                return kOfxStatOK;
            }

OfxStatus clipGetHandle(OfxImageEffectHandle imageEffect,
                            const char *name,
                            OfxImageClipHandle *clip,
                            OfxPropertySetHandle *propertySet){
                std::cout << "clipGetHandle" << std::endl;

                std::map<std::string, OfxImageClipHandle>::iterator it = imageEffect->clips.find(std::string(name));

                if (it != imageEffect->clips.end()) {
                    *clip = it->second;
                    if (propertySet != NULL) {
                        *propertySet = it->second->properties;
                    }
                } else {
                    return kOfxStatErrBadHandle;
                }

                return kOfxStatOK;
}
OfxStatus clipGetPropertySet(OfxImageClipHandle clip,
                     OfxPropertySetHandle *propHandle) {
                std::cout << "clipGetPropertySet" << std::endl;

                *propHandle = clip->properties;

                return kOfxStatOK;
}

OfxStatus clipGetImage(OfxImageClipHandle clip,
            OfxTime       time,
            const OfxRectD     *region,
            OfxPropertySetHandle   *imageHandle){
            std::cout << "clipGetImage" << std::endl;

            char* clipName;
            propGetString(clip->properties, "CLIP_NAME", 0, &clipName);

            Image* image = NULL;
            if (strcmp(clipName, kOfxImageEffectSimpleSourceClipName) == 0) {
                image = loadImage("/home/black/Downloads/image.ppm");
            }

            OfxPropertySetHandle clipImage = new OfxPropertySetStruct();
            propSetString(clipImage, kOfxImageEffectPropPixelDepth, 0, kOfxBitDepthByte);
            propSetString(clipImage, kOfxImageEffectPropComponents, 0, kOfxImageComponentRGBA);
            propSetString(clipImage, kOfxImageEffectPropPreMultiplication, 0, kOfxImageOpaque);
            propSetDouble(clipImage, kOfxImageEffectPropRenderScale, 0, 1.0);
            propSetDouble(clipImage, kOfxImageEffectPropRenderScale, 1, 1.0);
            propSetDouble(clipImage, kOfxImagePropPixelAspectRatio, 0, 1.0);

            int dimension[4] = {0, 0, 831, 530};
            if (image != NULL) {
                propSetPointer(clipImage, kOfxImagePropData, 0, image->data);
            } else {
                if (clip->data == NULL) {
                    clip->data = new char[dimension[2] * dimension[3] * 4];
                }
                propSetPointer(clipImage, kOfxImagePropData, 0, clip->data);
            }
            propSetIntN(clipImage, kOfxImagePropBounds, 4, dimension);
            propSetIntN(clipImage, kOfxImagePropRegionOfDefinition, 4, dimension);
            propSetInt(clipImage, kOfxImagePropRowBytes, 0, 4 * dimension[2]);
            propSetString(clipImage, kOfxImagePropField, 0, kOfxImageFieldNone);
            propSetString(clipImage, kOfxImagePropUniqueIdentifier, 0, "1");
            propSetString(clipImage, kOfxPropType, 0, kOfxTypeImage);

            *imageHandle = clipImage;

            return kOfxStatOK;
}
OfxStatus clipReleaseImage(OfxPropertySetHandle imageHandle){
                std::cout << "[!ERROR!] clipReleaseImage" << std::endl;

                //delete[] propGetPointer(imageHandle, kOfxImagePropData);
                return kOfxStatOK;
}

OfxStatus clipGetRegionOfDefinition(OfxImageClipHandle clip,
                    OfxTime time,
                    OfxRectD *bounds){
                std::cout << "clipGetRegionOfDefinition" << std::endl;

                bounds->x1 = 0;
                bounds->y1 = 0;
                bounds->x2 = 831;
                bounds->y2 = 530;


                return kOfxStatOK;
}
int abort(OfxImageEffectHandle imageEffect){
                //std::cout << "abort" << std::endl;
                return kOfxStatOK;
}
OfxStatus imageMemoryAlloc(OfxImageEffectHandle instanceHandle, 
            size_t nBytes,
            OfxImageMemoryHandle *memoryHandle){
                std::cout << "[!ERROR!] imageMemoryAlloc" << std::endl;
                return kOfxStatOK;
}
OfxStatus imageMemoryFree(OfxImageMemoryHandle memoryHandle){
                std::cout << "[!ERROR!] imageMemoryFree" << std::endl;
                return kOfxStatOK;
}
OfxStatus imageMemoryLock(OfxImageMemoryHandle memoryHandle,
                void **returnedPtr) {

                std::cout << "[!ERROR!] imageMemoryLock" << std::endl;
                return kOfxStatOK;
                }
OfxStatus imageMemoryUnlock(OfxImageMemoryHandle memoryHandle){
                std::cout << "[!ERROR!] imageMemoryUnlock" << std::endl;
                return kOfxStatOK;
}

OfxImageEffectSuiteV1* getOfxImageEffectSuiteV1() {
    if (effectSuite != NULL) {
        return effectSuite;
    } else {
        OfxImageEffectSuiteV1* effectSuite = new OfxImageEffectSuiteV1();
        effectSuite->abort = &abort;
        effectSuite->clipDefine = &clipDefine;
        effectSuite->clipGetHandle = &clipGetHandle;
        effectSuite->clipGetImage = &clipGetImage;
        effectSuite->clipGetPropertySet = &clipGetPropertySet;
        effectSuite->clipGetRegionOfDefinition = &clipGetRegionOfDefinition;
        effectSuite->clipReleaseImage = &clipReleaseImage;
        effectSuite->getParamSet = &getParamSet;
        effectSuite->getPropertySet = &getPropertySet;
        effectSuite->imageMemoryAlloc = &imageMemoryAlloc;
        effectSuite->imageMemoryFree = &imageMemoryFree;
        effectSuite->imageMemoryLock = &imageMemoryLock;
        effectSuite->imageMemoryUnlock = &imageMemoryUnlock;
        return effectSuite;
    }
}