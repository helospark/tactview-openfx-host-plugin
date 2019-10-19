#include "imageEffectStruct.h"
#include <iostream>
#include "paramSet.h"
#include "imageLoader.h"
#include <cstring>
#include "globalCallbackFunctions.h"
#include "global.h"

OfxImageEffectSuiteV1* effectSuite = NULL;

OfxImageEffectStruct::OfxImageEffectStruct() {
    properties = new OfxPropertySetStruct();
    parameters = new OfxParamSetStruct();
}

OfxImageEffectStruct::~OfxImageEffectStruct() {
    delete properties;
    delete parameters;
}

OfxStatus getPropertySet(OfxImageEffectHandle imageEffect, OfxPropertySetHandle *propHandle) {
    LOG("getPropertySet" );
    *propHandle = imageEffect->properties;
    return kOfxStatOK;
}

OfxStatus getParamSet(OfxImageEffectHandle imageEffect, OfxParamSetHandle *paramSet){
    LOG("getParamSet" );

    *paramSet = imageEffect->parameters;

    return kOfxStatOK;
}
OfxStatus clipDefine(OfxImageEffectHandle imageEffect,
            const char *name,	 
            OfxPropertySetHandle *propertySet) {
                LOG("clipDefine " << name );


                OfxPropertySetHandle clip = new OfxPropertySetStruct();
                propSetString(clip, "CLIP_NAME", 0, name);
                bool isAlpha = strcmp(name, "Mask") == 0;
               // const char* supportedComponents[] = {kOfxImageComponentRGBA, kOfxImageComponentAlpha};
                propSetString(clip, kOfxImageEffectPropComponents, 0, kOfxImageComponentRGBA);
              //  propSetStringN(clip, kOfxImageEffectPropSupportedComponents, 2, supportedComponents);

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

                LOG("Clip type" << typeToUse );

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
                LOG("clipGetHandle " << name );

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
                LOG("clipGetPropertySet" );

                *propHandle = clip->properties;

                return kOfxStatOK;
}

void* convertImage(Image* image, char* type, char* componentType, bool* allocated, int* numberOfComponentsOut, int* outputSize) {
    if (image == NULL) {
        return NULL;
    }
    void* data = NULL;
    if (strcmp(type, kOfxBitDepthByte) == 0 && strcmp(componentType, kOfxImageComponentRGBA) == 0) {
        *outputSize = image->width * image->height * 4 * sizeof(char);
        data = image->data;
        *allocated = false;
        *numberOfComponentsOut = 4;

        return data;
    } else {
        *allocated = true;
        int numberOfComponents = 1;
        if (strcmp(componentType, kOfxImageComponentRGBA) == 0) {
            numberOfComponents = 4;
        } else if (strcmp(componentType, kOfxImageComponentRGB) == 0) {
            numberOfComponents = 3;
        } else if (strcmp(componentType, kOfxImageComponentAlpha) == 0) {
            numberOfComponents = 1;
        } else {
            LOG_ERROR("Unknown number of components" );
        }

        *numberOfComponentsOut = numberOfComponents;

        int numberOfElements = image->width * image->height * numberOfComponents;
        if (strcmp(type, kOfxBitDepthByte) == 0) {
            char* result = new char[numberOfElements];
            for (int i = 0; i < image->width * image->height; ++i) {
                for (int j = 0; j < numberOfComponents; ++j) {
                    result[i * numberOfComponents + j] = ((char*)image->data)[i * 4 + j];
                }
            }
            *outputSize = numberOfElements * sizeof(char);
            return result;
        }
        else if (strcmp(type, kOfxBitDepthFloat) == 0) {
            float* result = new float[numberOfElements];
            for (int i = 0; i < image->width * image->height; ++i) {
                for (int j = 0; j < numberOfComponents; ++j) {
                    result[i * numberOfComponents + j] = ((char*)image->data)[i * 4 + j] / 255.0f;
                }
            }
            *outputSize = numberOfElements * sizeof(float);
            return result;
        } else {
            LOG_ERROR("Unsupported type " << type );
        }


    }
    return NULL;
}

OfxStatus clipGetImage(OfxImageClipHandle clip,
            OfxTime       time,
            const OfxRectD     *region,
            OfxPropertySetHandle   *imageHandle){
            LOG("clipGetImage" );

            char* clipName;
            propGetString(clip->properties, "CLIP_NAME", 0, &clipName);

            char* type = clip->properties->strings[kOfxImageEffectPropPixelDepth][0];
            int sizePerComponent = 1;
            if (strcmp(type, kOfxBitDepthByte) == 0) {
                sizePerComponent = sizeof(char);
            } else {
                sizePerComponent = sizeof(float);
            }

            char* componentType = NULL;
            for (auto element : clip->properties->strings[kOfxImageEffectPropComponents]) {
                if (strcmp(element, kOfxParamTypeRGBA) == 0) {
                    componentType = kOfxParamTypeRGBA;
                    break;
                }
            }
            if (componentType == NULL) {
                componentType = clip->properties->strings[kOfxImageEffectPropComponents][0];
            }

            int width = clip->imageEffect->currentRenderRequest->width;
            int height = clip->imageEffect->currentRenderRequest->height;

            void* data;
            int dataSize;
            bool allocated = false;
            int numberOfComponents = 0;
            
            if (strcmp(clipName, kOfxImageEffectOutputClipName) == 0) {
                dataSize = width * height * sizePerComponent * 4;
                numberOfComponents = 4;
                if (clip->dataSize != dataSize) {
                    delete[] clip->data;
                    data = new char[dataSize];
                } else {
                    data = clip->data;
                }
            } else {

                auto source = clip->imageEffect->currentRenderRequest->sourceClips.find(clipName);

                if (source != clip->imageEffect->currentRenderRequest->sourceClips.end()) {
                    if (time == clip->imageEffect->currentRenderRequest->time) {
                        data = convertImage(source->second, type, componentType, &allocated, &numberOfComponents, &dataSize);
                        width = source->second->width;
                        height = source->second->height;
                    } else {


                        LoadImageRequest loadImageRequest;
                        loadImageRequest.time = time;
                        loadImageRequest.width = width;
                        loadImageRequest.height = height;
                        loadImageRequest.clipName = clipName;
                        loadImageRequest.scale = clip->imageEffect->currentRenderRequest->scale;
                        loadImageRequest.effectId = clip->imageEffect->currentRenderRequest->effectId;
                        loadImageRequest.data = NULL;

                        LOG("About to call Java" );

                        globalFunctionPointers->loadImageCallback(&loadImageRequest);

                        LOG("Source image loaded " );
                        LOG(loadImageRequest.width << " " << loadImageRequest.height );
                        LOG(width << " " << height );
                        LOG((int*)loadImageRequest.data );

                        if (loadImageRequest.data) {
                            Image image(loadImageRequest.width, loadImageRequest.height, loadImageRequest.data) ;
                            data = convertImage(&image, type, componentType, &allocated, &numberOfComponents, &dataSize);

                            width = loadImageRequest.width;
                            height = loadImageRequest.height;

                            if (allocated) {
                                // free original buffer
                            }
                        } else {
                            return kOfxStatFailed;
                        }
                    }
                } else {
                    return kOfxStatFailed;
                }

            }


            clip->data = data;
            clip->dataSize = dataSize;
            clip->allocated = allocated;

            OfxPropertySetHandle clipImage = new OfxPropertySetStruct();
            propSetString(clipImage, kOfxImageEffectPropComponents, 0, componentType);
            propSetString(clipImage, kOfxImageEffectPropPreMultiplication, 0, kOfxImageOpaque);
            propSetDouble(clipImage, kOfxImageEffectPropRenderScale, 0, 1.0);
            propSetDouble(clipImage, kOfxImageEffectPropRenderScale, 1, 1.0);
            propSetDouble(clipImage, kOfxImagePropPixelAspectRatio, 0, 1.0);

            int dimension[4] = {0, 0, width, height};

            propSetPointer(clipImage, kOfxImagePropData, 0, data);
            propSetIntN(clipImage, kOfxImagePropBounds, 4, dimension);
            propSetIntN(clipImage, kOfxImagePropRegionOfDefinition, 4, dimension);
            propSetInt(clipImage, kOfxImagePropRowBytes, 0, sizePerComponent * numberOfComponents * dimension[2]);
            propSetString(clipImage, kOfxImagePropField, 0, kOfxImageFieldNone);
            propSetString(clipImage, kOfxImagePropUniqueIdentifier, 0, "1");
            propSetString(clipImage, kOfxPropType, 0, kOfxTypeImage);
            propSetString(clipImage, kOfxImageEffectPropPixelDepth, 0, type);
            propSetInt(clipImage, "ALLOCATED", 0, allocated ? 1 : 0);

            *imageHandle = clipImage;

            return kOfxStatOK;
}
OfxStatus clipReleaseImage(OfxPropertySetHandle imageHandle){
                LOG("clipReleaseImage" );
                int allocated;
                propGetInt(imageHandle, "ALLOCATED", 0, &allocated);

                if (allocated) {
                    LOG("Deleting allocated memory " );

                    void* pointer;
                    propGetPointer(imageHandle, kOfxImagePropData, 0, &pointer);
                    delete[] pointer;
                }

                delete imageHandle;

                return kOfxStatOK;
}

OfxStatus clipGetRegionOfDefinition(OfxImageClipHandle clip,
                    OfxTime time,
                    OfxRectD *bounds){
                LOG("clipGetRegionOfDefinition" );

                bounds->x1 = 0;
                bounds->y1 = 0;
                bounds->x2 = clip->imageEffect->currentRenderRequest->width;
                bounds->y2 = clip->imageEffect->currentRenderRequest->height;


                return kOfxStatOK;
}


int abortInternal(OfxImageEffectHandle imageEffect){
    LOG("abort" );
    return kOfxStatOK;
}

OfxStatus imageMemoryAlloc(OfxImageEffectHandle instanceHandle, 
            size_t nBytes,
            OfxImageMemoryHandle *memoryHandle){
                LOG("imageMemoryAlloc" );

                OfxImageMemoryStruct* result = new OfxImageMemoryStruct();
                result->memory = new char[nBytes];

                *memoryHandle = result;

                return kOfxStatOK;
}

OfxStatus imageMemoryFree(OfxImageMemoryHandle memoryHandle){
                LOG("imageMemoryFree" );

                delete[] memoryHandle->memory;
                delete memoryHandle;

                return kOfxStatOK;
}

OfxStatus imageMemoryLock(OfxImageMemoryHandle memoryHandle,
                void **returnedPtr) {
                LOG("imageMemoryLock" );

                *returnedPtr = memoryHandle->memory;

                return kOfxStatOK;
}

OfxStatus imageMemoryUnlock(OfxImageMemoryHandle memoryHandle){
                LOG("imageMemoryUnlock" );
                // noop
                return kOfxStatOK;
}

OfxImageEffectSuiteV1* getOfxImageEffectSuiteV1() {
    if (effectSuite != NULL) {
        return effectSuite;
    } else {
        effectSuite = new OfxImageEffectSuiteV1();
        effectSuite->abort = &abortInternal;
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