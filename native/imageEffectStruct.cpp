#include "imageEffectStruct.h"
#include <iostream>
#include "paramSet.h"
#include "imageLoader.h"
#include <cstring>
#include "globalCallbackFunctions.h"

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
                bool isAlpha = strcmp(name, "Mask") == 0;
                if (!isAlpha) {
                    const char* supportedComponents[] = {"kOfxImageComponentRGBA", "kOfxImageComponentAlpha"};
                    propSetString(clip, kOfxImageEffectPropComponents, 0, kOfxImageComponentRGBA);
                    propSetStringN(clip, kOfxImageEffectPropSupportedComponents, 2, supportedComponents);
                } else {
                    const char* supportedComponents[] = {"kOfxImageComponentAlpha"};
                    propSetString(clip, kOfxImageEffectPropComponents, 0, kOfxImageComponentAlpha);
                    propSetStringN(clip, kOfxImageEffectPropSupportedComponents, 2, supportedComponents);
                }

                auto actualSupportedFormats = imageEffect->properties->strings[kOfxImageEffectPropSupportedPixelDepths];

                char* typeToUse = NULL;

                for (auto& a : actualSupportedFormats) {
                    if (strcmp(a, kOfxBitDepthByte) == 0) {
                        typeToUse = kOfxBitDepthByte;
                        break;
                    }
                }
                if (!typeToUse) {
                    typeToUse = imageEffect->properties->strings[kOfxImageEffectPropSupportedPixelDepths][0];
                }
                /**
                if (imageEffect->properties->strings.find(kOfxImageEffectPropSupportedPixelDepths) != imageEffect->properties->strings.end()) {
                    auto supportedFormats = imageEffect->properties->strings[kOfxImageEffectPropSupportedPixelDepths];
                    bool supports = false;
                    for (auto element : supportedFormats) {
                        if (strcmp(element, typeToUse) == 0) {
                            supports = true;
                            break;
                        }
                    }

                    if (!supports) {
                           typeToUse = new char[100];
                           propGetString(imageEffect->properties, kOfxActionDestroyInstanceInteract, 0, &typeToUse);
                           std::cout << "Type overridden globally to " << typeToUse << std::endl;
                    }
                } */

                std::cout << "Clip type" << typeToUse << std::endl;

                propSetString(clip, kOfxImageEffectPropPixelDepth, 0, typeToUse);
                propSetString(clip, "CLIP_TYPE", 0, typeToUse);
                propSetInt(clip, kOfxImageClipPropConnected, 0, 1);


                OfxImageClipStruct* clipStruct = new OfxImageClipStruct();
                clipStruct->properties = clip;
                clipStruct->imageEffect = imageEffect;

                imageEffect->clips[std::string(name)] = clipStruct;

                *propertySet = clip;

                return kOfxStatOK;
            }

OfxStatus clipGetHandle(OfxImageEffectHandle imageEffect,
                            const char *name,
                            OfxImageClipHandle *clip,
                            OfxPropertySetHandle *propertySet){
                std::cout << "clipGetHandle " << name << std::endl;

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

void createMask(OfxImageClipHandle clip,
            OfxTime       time,
            const OfxRectD     *region,
            OfxPropertySetHandle   *imageHandle) {

            OfxPropertySetHandle clipImage = new OfxPropertySetStruct();
            propSetString(clipImage, kOfxImageEffectPropPixelDepth, 0, kOfxBitDepthByte);
            propSetString(clipImage, kOfxImageEffectPropComponents, 0, kOfxImageComponentAlpha);
            propSetString(clipImage, kOfxImageEffectPropPreMultiplication, 0, kOfxImageOpaque);
            propSetDouble(clipImage, kOfxImageEffectPropRenderScale, 0, 1.0);
            propSetDouble(clipImage, kOfxImageEffectPropRenderScale, 1, 1.0);
            propSetDouble(clipImage, kOfxImagePropPixelAspectRatio, 0, 1.0);
            propSetString(clipImage, kOfxImageEffectPropPixelDepth, 0, clip->properties->strings["CLIP_TYPE"][0]);

            int dataSize;
            int dimension[4] = {0, 0, 831, 530};
            if (strcmp(clip->properties->strings["CLIP_TYPE"][0], kOfxBitDepthByte) == 0) {
                dataSize = 1;
                if (clip->data == NULL) {
                    int size = dimension[2] * dimension[3] * dataSize;
                    char* clipData = new char[size];
                    for (int i = 0; i < size; ++i) {
                        clipData[i] = (char)255;
                    }
                    clip->data = clipData;
                }
            } else if (strcmp(clip->properties->strings["CLIP_TYPE"][0], kOfxBitDepthFloat) == 0) {
                dataSize = 4;
                    if (clip->data == NULL) {
                        int size = dimension[2] * dimension[3] * sizeof(float);
                        float* clipData = new float[size];
                        for (int i = 0; i < size; ++i) {
                            clipData[i] = 1.0;
                        }
                        clip->data = clipData;
                    }
            } else {
                std::cout << "[!Error!] No conversion to image type" << std::endl;
            }

            propSetInt(clipImage, kOfxImagePropRowBytes, 0, dataSize * dimension[2]);
            propSetPointer(clipImage, kOfxImagePropData, 0, clip->data);
            propSetIntN(clipImage, kOfxImagePropBounds, 4, dimension);
            propSetIntN(clipImage, kOfxImagePropRegionOfDefinition, 4, dimension);
            propSetString(clipImage, kOfxImagePropField, 0, kOfxImageFieldNone);
            propSetString(clipImage, kOfxImagePropUniqueIdentifier, 0, "1");
            propSetString(clipImage, kOfxPropType, 0, kOfxTypeImage);

            *imageHandle = clipImage;
}

void* convertImage(Image* image, char* type, int* outputSize) {
    if (image == NULL) {
        return NULL;
    }
    if (strcmp(type, kOfxBitDepthByte) == 0) {
        *outputSize = image->width * image->height * sizeof(char);
        return image->data;
    } else if (strcmp(type, kOfxBitDepthFloat) == 0) {

        float* newData = new float[image->width * image->height * sizeof(float)];
        for (int i = 0; i < image->width * image->height * 4; ++i) {
            newData[i] = (((unsigned char*)image->data)[i]) / 255.0f;
        }
        *outputSize = image->width * image->height * sizeof(float);
        delete[] image->data;
        return newData;
    } else {
        std::cout << "[!Error!] cannot convert, unknown type0" << std::endl;
    }
    return NULL;
}

OfxStatus clipGetImage(OfxImageClipHandle clip,
            OfxTime       time,
            const OfxRectD     *region,
            OfxPropertySetHandle   *imageHandle){
            std::cout << "clipGetImage" << std::endl;

            char* clipName;
            propGetString(clip->properties, "CLIP_NAME", 0, &clipName);

            if (strcmp(clipName, "Mask") == 0) {
                std::cout << "Creating white mask" << std::endl;
                createMask(clip, time, region, imageHandle);
                return kOfxStatOK;
            }


            Image* image = NULL;


            char* type = clip->properties->strings[kOfxImageEffectPropPixelDepth][0];
            int sizePerComponent = 1;
            if (strcmp(type, kOfxBitDepthByte) == 0) {
                sizePerComponent = 1;
            } else {
                sizePerComponent = 4;
            }


            int width = clip->imageEffect->currentRenderRequest->width;
            int height = clip->imageEffect->currentRenderRequest->height;
            void* data;
            int dataSize;
            if (strcmp(clipName, kOfxImageEffectSimpleSourceClipName) == 0) {
                LoadImageRequest loadImageRequest;
                loadImageRequest.time = time;
                loadImageRequest.width = width;
                loadImageRequest.height = height;
                loadImageRequest.data = NULL;

                std::cout << "About to call Java" << std::endl;


                globalFunctionPointers->loadImageCallback(&loadImageRequest);

                std::cout << "Source image loaded " << std::endl;
                std::cout << loadImageRequest.width << " " << loadImageRequest.height << std::endl;

                image = new Image(loadImageRequest.width, loadImageRequest.height, loadImageRequest.data);

                data = convertImage(image, type, &dataSize);
            } else if (strcmp(clipName, kOfxImageEffectOutputClipName) == 0) {
                dataSize = width * height * sizePerComponent * 4;
                if (clip->dataSize != dataSize) {
                    delete[] clip->data;
                    data = new char[dataSize];
                }
            }


            clip->data = data;
            clip->dataSize = dataSize;

            OfxPropertySetHandle clipImage = new OfxPropertySetStruct();
            propSetString(clipImage, kOfxImageEffectPropComponents, 0, kOfxImageComponentRGBA);
            propSetString(clipImage, kOfxImageEffectPropPreMultiplication, 0, kOfxImageOpaque);
            propSetDouble(clipImage, kOfxImageEffectPropRenderScale, 0, 1.0);
            propSetDouble(clipImage, kOfxImageEffectPropRenderScale, 1, 1.0);
            propSetDouble(clipImage, kOfxImagePropPixelAspectRatio, 0, 1.0);

            int dimension[4] = {0, 0, width, height};

            propSetPointer(clipImage, kOfxImagePropData, 0, data);
            propSetIntN(clipImage, kOfxImagePropBounds, 4, dimension);
            propSetIntN(clipImage, kOfxImagePropRegionOfDefinition, 4, dimension);
            propSetInt(clipImage, kOfxImagePropRowBytes, 0, sizePerComponent * 4 * dimension[2]);
            propSetString(clipImage, kOfxImagePropField, 0, kOfxImageFieldNone);
            propSetString(clipImage, kOfxImagePropUniqueIdentifier, 0, "1");
            propSetString(clipImage, kOfxPropType, 0, kOfxTypeImage);
            propSetString(clipImage, kOfxImageEffectPropPixelDepth, 0, type);

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
                bounds->x2 = clip->imageEffect->currentRenderRequest->width;
                bounds->y2 = clip->imageEffect->currentRenderRequest->height;


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