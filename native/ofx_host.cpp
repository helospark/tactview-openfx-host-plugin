#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../openfx/include/ofxCore.h"
#include <iostream>
#include "imageEffectStruct.h"
#include "ofx_property.h"
#include "paramSet.h"
#include "imageLoader.h"
#include "multithread.h"
#include "memorySuite.h"
#include "messageSuite.h"
#include <sstream>
#include <map>
#include "globalCallbackFunctions.h"
#include "string_operations.h"

#ifdef __linux__ 
#include <dlfcn.h>
#elif _WIN32 || __CYGWIN__
#include <windows.h>
#endif

typedef int(*intFptr)();
typedef OfxPlugin*(*getPluginFptr)(int);

const void *fetchSuite(OfxPropertySetHandle host, const char *suiteName, int suiteVersion) {
    std::cout << "Getting " << suiteName << " " << suiteVersion << std::endl;

    if (strcmp(suiteName, kOfxImageEffectSuite) == 0 && suiteVersion == 1) {
        return getOfxImageEffectSuiteV1();
    } else if (strcmp(suiteName, kOfxPropertySuite) == 0 && suiteVersion == 1) {
        return createPropertySuiteV1();
    } else if (strcmp(suiteName, kOfxParameterSuite) == 0 && suiteVersion == 1) {
        return getParameterSuite();
    } else if (strcmp(suiteName, kOfxMultiThreadSuite) == 0 && suiteVersion == 1) {
        return getMultiThreadSuite();
    } else if (strcmp(suiteName, kOfxMemorySuite) == 0 && suiteVersion == 1) {
        return getMemorySuiteV1();
    } else if (strcmp(suiteName, kOfxMessageSuite) == 0 && suiteVersion <= 2) {
        return getMessageSuite();
    }

    std::cout << " :( [ERROR] missing feature" << std::endl;

    return (void*)0;
}

OfxStatus callEntryPoint(const char *action, const void *handle, OfxPropertySetHandle inArgs, OfxPropertySetHandle outArgs, OfxPlugin* plugin) {
    std::cout << "Calling " << action << std::endl;
    OfxStatus result = plugin->mainEntry(action, handle, inArgs, outArgs);   
    std::cout << action << " done" << std::endl;
    if (result != kOfxStatOK) {
        std::cout << "Error, result is " << result << std::endl;
    }
    return result;
}

struct PluginDefinition {
    OfxImageEffectHandle effectHandle;
    OfxPlugin* ofxPlugin;
    int pluginIndex;

    char* pluginName;
    char* pluginDescription;
};

std::map<int, PluginDefinition*> createdPlugins;
std::map<std::string, void*> handles;

std::vector<PluginDefinition> loadDefinitions() {

}

struct LoadPluginRequest {
    int libraryDescriptor;
    int pluginIndex;

    int width;
    int height;
};

OfxHost* globalHost = NULL;

OfxHost* createGlobalHost() {
    OfxHost* host = new OfxHost();
    host->fetchSuite = &fetchSuite;
    host->host = new OfxPropertySetStruct();
    propSetString(host->host, kOfxPropName, 0, "com.helospark.tactview");
    propSetString(host->host, kOfxPropLabel, 0, "Tactview");
    propSetString(host->host, kOfxPropVersion, 0, "1.0");
    propSetString(host->host, kOfxPropVersionLabel, 0, "1.0");
    propSetInt(host->host, kOfxImageEffectHostPropIsBackground, 0, 0);
    propSetInt(host->host, kOfxImageEffectPropSupportsOverlays, 0, 0);
    propSetInt(host->host, kOfxImageEffectPropSupportsMultiResolution, 0, 1);
    propSetInt(host->host, kOfxImageEffectPropSupportsTiles, 0, 0);
    propSetInt(host->host, kOfxImageEffectPropTemporalClipAccess, 0, 1);

    const char* supportedComponents[] = {kOfxImageComponentRGBA, kOfxImageComponentAlpha};
    propSetStringN(host->host, kOfxImageEffectPropSupportedComponents, 2, supportedComponents);
    
    const char* supportedContexts[] = {kOfxImageEffectContextGenerator, kOfxImageEffectContextFilter};
    propSetStringN(host->host, kOfxImageEffectPropSupportedContexts, 2, supportedContexts);
    
    propSetInt(host->host, kOfxImageEffectPropSupportsMultipleClipDepths, 0, 0);
    propSetInt(host->host, kOfxImageEffectPropSupportsMultipleClipPARs, 0, 0);
    propSetInt(host->host, kOfxImageEffectPropSetableFrameRate, 0, 0);
    propSetInt(host->host, kOfxImageEffectPropSetableFielding, 0, 0);
    propSetInt(host->host, kOfxParamHostPropSupportsCustomInteract, 0, 0);
    propSetInt(host->host, kOfxParamHostPropSupportsStringAnimation, 0, 1);
    propSetInt(host->host, kOfxParamHostPropSupportsChoiceAnimation, 0, 1);
    propSetInt(host->host, kOfxParamHostPropSupportsBooleanAnimation, 0, 1);
    propSetInt(host->host, kOfxParamHostPropSupportsCustomAnimation, 0, 0);
    propSetInt(host->host, kOfxParamHostPropMaxParameters, 0, 100);
    propSetInt(host->host, kOfxParamHostPropMaxPages, 0, 10);
    const char* supportedPixelDepths[] = {kOfxBitDepthByte, kOfxBitDepthFloat};
    propSetStringN(host->host, kOfxImageEffectPropSupportedPixelDepths, 1, supportedPixelDepths);
    int rowColumnCount[2] = {10, 1};
    propSetIntN(host->host, kOfxParamHostPropPageRowColumnCount, 2, rowColumnCount);
    propSetPointer(host->host, kOfxPropHostOSHandle, 0, NULL);
    propSetInt(host->host, kOfxImageEffectInstancePropSequentialRender, 0, 0);
    propSetInt(host->host, kOfxImageEffectPropRenderQualityDraft, 0, 1);
    propSetString(host->host, kOfxPropVersion, 0, "1.0");

    return host;
}

extern "C" {

struct InitializeHostRequest {
    LoadImageCallback loadImageCallback;
    ParameterValueProviderCallback parameterValueProviderCallback;
};

void initializeHost(InitializeHostRequest* request) {
    globalHost = createGlobalHost();

    globalFunctionPointers = new GlobalFunctions();
    globalFunctionPointers->loadImageCallback = request->loadImageCallback;
    globalFunctionPointers->resolveParameterCallback = request->parameterValueProviderCallback;
}

struct LoadedLibraryDescriptor {
    getPluginFptr getPlugin;
    char* file;
    void* handle;
};

std::map<int, LoadedLibraryDescriptor*> loadedLibraries;
int loadedLibraryIndex = 0;

struct LoadLibraryRequest {
    const char* file;

    int numberOfPlugins;
};

int loadLibrary(LoadLibraryRequest* loadLibraryRequest) {

    void *handle;
    
    const char* file = loadLibraryRequest->file; 

    #ifdef __linux__
        handle = dlopen(file, RTLD_LAZY);
        if (!handle) {
            fprintf(stderr, "Error: %s\n", dlerror());
            return EXIT_FAILURE;
        }
    #elif _WIN32 || __CYGWIN__
        handle = LoadLibraryA(file); 
        if (!handle) {
            fprintf(stderr, "Error: %d\n", GetLastError());
            return EXIT_FAILURE;
        }
    #endif

    int k = 2;

    intFptr func_print_name = NULL;

    #ifdef __linux__
        func_print_name = (intFptr) dlsym(handle, "OfxGetNumberOfPlugins");
        if (!func_print_name) {
            /* no such symbol */
            fprintf(stderr, "Error: %s\n", dlerror());
            dlclose(handle);
            return EXIT_FAILURE;
        }
    #elif _WIN32 || __CYGWIN__
        func_print_name = (intFptr) GetProcAddress((HINSTANCE)handle, "OfxGetNumberOfPlugins");
        if (!func_print_name) {
            /* no such symbol */
            fprintf(stderr, "Error: ???\n");
            //dlclose(handle);
            return EXIT_FAILURE;
        }
    #endif
    int plugins = func_print_name();
    fprintf(stderr, "Loaded, number of plugins %d\n", plugins);

    getPluginFptr getPlugin = NULL;
    #ifdef __linux__
        getPlugin = (getPluginFptr) dlsym(handle, "OfxGetPlugin");
    #elif _WIN32 || __CYGWIN__
        getPlugin = (getPluginFptr) GetProcAddress((HINSTANCE)handle, "OfxGetPlugin");
    #endif

    loadLibraryRequest->numberOfPlugins = plugins;

    LoadedLibraryDescriptor* descriptor = new LoadedLibraryDescriptor();
    descriptor->getPlugin = getPlugin;
    descriptor->file = duplicateString(loadLibraryRequest->file);
    descriptor->handle = handle;

    int descriptorIndex = loadedLibraryIndex++;

    loadedLibraries[descriptorIndex] = descriptor;

    return descriptorIndex;
}

int globalUniquePluginIndex = 0;

struct LoadedPluginDescriptor {
    OfxPlugin* plugin;
    int libraryDescriptor;
    OfxImageEffectHandle effectDescriptor;
    std::vector<char*> supportedContexts;
    

    char* name;
};

int globalUniqueLoadedPluginIndex = 0;

std::map<int, LoadedPluginDescriptor*> pluginDescriptors;

int loadPlugin(LoadPluginRequest* loadPluginRequest) {
    LoadedLibraryDescriptor* loadedLibraryDescriptor = loadedLibraries[loadPluginRequest->libraryDescriptor];

    OfxPlugin* plugin = loadedLibraryDescriptor->getPlugin(loadPluginRequest->pluginIndex);

    std::cout << "Api version = " << plugin->apiVersion << " plugin_index=" << loadPluginRequest->pluginIndex << std::endl;

    plugin->setHost(globalHost);

    callEntryPoint(kOfxActionLoad, NULL, NULL, NULL, plugin);


    OfxImageEffectHandle effectHandle = new OfxImageEffectStruct();
    propSetString(effectHandle->properties, kOfxPropType, 0, kOfxTypeImageEffect);
    propSetString(effectHandle->properties, kOfxPropLabel, 0, "label");
    propSetString(effectHandle->properties, kOfxPropShortLabel, 0, "label");
    propSetString(effectHandle->properties, kOfxPropLongLabel, 0, "label");
    propSetString(effectHandle->properties, kOfxPropVersion, 0, "1");
    propSetString(effectHandle->properties, kOfxPropVersionLabel, 0, "1");
    propSetString(effectHandle->properties, kOfxPluginPropFilePath, 0, loadedLibraryDescriptor->file);
    propSetString(effectHandle->properties, kOfxImageEffectPropSupportedContexts, 0, kOfxImageEffectContextGeneral);
    propSetString(effectHandle->properties, kOfxImageEffectPropSupportedPixelDepths, 0, kOfxBitDepthByte);

    propSetDouble(effectHandle->properties, kOfxImageEffectPropProjectExtent, 0, loadPluginRequest->width);
    propSetDouble(effectHandle->properties, kOfxImageEffectPropProjectExtent, 1, loadPluginRequest->height);

    propSetDouble(effectHandle->properties, kOfxImageEffectPropProjectSize, 0, loadPluginRequest->width);
    propSetDouble(effectHandle->properties, kOfxImageEffectPropProjectSize, 1, loadPluginRequest->height);

    propSetDouble(effectHandle->properties, kOfxImageEffectPropProjectOffset, 0, 0);
    propSetDouble(effectHandle->properties, kOfxImageEffectPropProjectOffset, 1, 0);


    LoadedPluginDescriptor* globalPluginDescriptor = new LoadedPluginDescriptor();
    globalPluginDescriptor->plugin = plugin;
    globalPluginDescriptor->libraryDescriptor = loadPluginRequest->libraryDescriptor;
    globalPluginDescriptor->effectDescriptor = effectHandle;

    globalUniqueLoadedPluginIndex++;
    pluginDescriptors[globalUniqueLoadedPluginIndex] = globalPluginDescriptor;

    return globalUniqueLoadedPluginIndex;
}

struct CreatePluginInstanceRequest {
    int width;
    int height;

    int loadedPluginIndex;
};

struct RenderImageRequest {
    int width;
    int height;
    int pluginIndex;
    double time;
    char* returnValue;
    char* inputImage;
};


int charToUnsignedInt(char data) {
    int iData = (int)data;
    if (data < 0) {
        return iData + 256;
    } else {
        return iData;
    }
}

int clamp(int v, int min, int max) {
    if (v < min) {
        return min;
    }
    if (v > max) {
        return max;
    }
    return v;
}

struct DescribeRequest {
    int pluginIndex;

    char* name;
    char* description;
    int supportedContextSize;
    char** supportedContexts;
};

void describe(DescribeRequest* describeRequest) {
    LoadedPluginDescriptor* pluginDefinition = pluginDescriptors[describeRequest->pluginIndex];
    OfxImageEffectHandle effectHandle = pluginDefinition->effectDescriptor;

    std::cout << "Calling entrypoint" << std::endl;

    callEntryPoint(kOfxActionDescribe, effectHandle, NULL, NULL, pluginDefinition->plugin);
    
    std::cout << "!DESCRIBE! Returning back" << std::endl;

    printAllProperties(effectHandle->properties);

    propGetString(effectHandle->properties, kOfxPropLabel, 0, &pluginDefinition->name);
    //propGetString(effectHandle->properties, kOfxPropPluginDescription, 0, &pluginDefinition->pluginDescription);

    for (int i = 0; i < effectHandle->properties->strings[kOfxImageEffectPropSupportedContexts].size(); ++i) {
        char* pointer;
        propGetString(effectHandle->properties, kOfxImageEffectPropSupportedContexts, i, &pointer);
        pluginDefinition->supportedContexts.push_back(pointer);
    }

    describeRequest->name = pluginDefinition->name;
    describeRequest->description = "";// pluginDefinition->pluginDescription;
    describeRequest->supportedContexts = &pluginDefinition->supportedContexts[0];
    describeRequest->supportedContextSize = pluginDefinition->supportedContexts.size();
}

struct ParameterMap {
    int numberOfValues;
    const char* key;
    char** value;
};

struct Parameter {
    int numberOfEntries;
    ParameterMap* parameterMap;
    int uniqueParameterId;
    const char* name;
    const char* type;
};

struct ParameterList {
    int numberOfParameters;
    Parameter* parameter;
};


struct DescribeInContextRequest {
    int pluginIndex;
};

int describeInContext(DescribeInContextRequest* describeInContextRequest) {
    LoadedPluginDescriptor* pluginDefinition = pluginDescriptors[describeInContextRequest->pluginIndex];
    OfxImageEffectHandle effectHandle = pluginDefinition->effectDescriptor;

    OfxPropertySetHandle inParam = new OfxPropertySetStruct();

    propSetString(effectHandle->properties, kOfxImageEffectPropContext, 0, kOfxImageEffectContextFilter);
    propSetString(effectHandle->properties, kOfxImageEffectPropComponents, 0, kOfxImageComponentRGB);
    propSetString(inParam, kOfxImageEffectPropContext, 0, kOfxImageEffectContextFilter);


    OfxStatus status = callEntryPoint(kOfxImageEffectActionDescribeInContext, effectHandle, inParam, NULL, pluginDefinition->plugin);

    delete inParam;

    bool returnStatus = 0;
    if (status == kOfxStatErrUnsupported) {
        returnStatus = -1;
    }
    
    return returnStatus;
}

struct CreateInstanceRequest {
    int pluginIndex;
    int width;
    int height;
    char* effectId;
    ParameterList* list;
};

int globalParameterIndex = 0;

OfxImageClipHandle copyClip(OfxImageClipStruct* input, OfxImageEffectHandle newHandle) {
    OfxImageClipStruct* clip = new OfxImageClipStruct();
    clip->type = input->type;
    clip->properties = copyProperties(input->properties);
    clip->imageEffect = newHandle;
    clip->data = NULL;
    clip->dataSize = 0;
}

OfxImageEffectHandle copyImageEffectHandle(OfxImageEffectHandle from) {
    OfxImageEffectHandle actualHandle = new OfxImageEffectStruct();

    actualHandle->describeInContextList = NULL;
    actualHandle->properties = copyProperties(from->properties);

    // copy parameters
    actualHandle->parameters = new OfxParamSetStruct();
    OfxParamSetStruct* parameters = from->parameters;
    for (auto element : parameters->parameters) {
        OfxParamStruct* copiedParameter = new OfxParamStruct(element->name, element->type);
        copiedParameter->properties = element->properties;
        copiedParameter->paramId = globalParameterIndex++;

        std::cout << "Creating new parameter " << copiedParameter->name << " " << globalParameterIndex << std::endl;

        actualHandle->parameters->parameters.push_back(copiedParameter);
    }

    // copy clips
    std::map<std::string, OfxImageClipStruct*> originalClips = from->clips;

    for (auto element : originalClips) {
        actualHandle->clips[element.first] = copyClip(element.second, actualHandle);
    }
    return actualHandle;
}

void deleteImageEffectHandle(OfxImageEffectHandle toDelete) {
    delete toDelete->properties;
    for (auto element : toDelete->parameters->parameters) {
        delete element;
    }
    for (auto element : toDelete->clips) {
        delete element.second->properties;
        delete element.second;
    }
}

int createInstance(CreateInstanceRequest* request) {
    LoadedPluginDescriptor* pluginDescriptor = pluginDescriptors[request->pluginIndex];
    LoadedLibraryDescriptor* loadedLibraryDescriptor = loadedLibraries[pluginDescriptor->libraryDescriptor];

    OfxImageEffectHandle instanceHandle = copyImageEffectHandle(pluginDescriptor->effectDescriptor);
    
    callEntryPoint(kOfxActionCreateInstance, instanceHandle, NULL, NULL, pluginDescriptor->plugin);

    pluginDescriptor->effectDescriptor->effectId = duplicateString(request->effectId);

    PluginDefinition* pluginDefinition = new PluginDefinition();
    pluginDefinition->effectHandle = instanceHandle;
    pluginDefinition->pluginIndex = request->pluginIndex;
    pluginDefinition->ofxPlugin = pluginDescriptor->plugin;

    globalUniquePluginIndex++;
    createdPlugins[globalUniquePluginIndex] = pluginDefinition;



    int numberOfParameters = instanceHandle->parameters->parameters.size();
    request->list = new ParameterList();
    request->list->numberOfParameters = numberOfParameters;
    request->list->parameter = new Parameter[numberOfParameters];


    for (int i = 0; i < numberOfParameters; ++i) {
        OfxParamStruct* parameterHandle = instanceHandle->parameters->parameters[i];
        std::map<std::string, std::vector<char*>>* parameterMapPointer = getParametersAsMap(parameterHandle->properties);
        std::map<std::string, std::vector<char*>>& parameterMap = *parameterMapPointer;

        auto& parameter = request->list->parameter[i];
        
        parameter.numberOfEntries = parameterMap.size();
        parameter.parameterMap = new ParameterMap[parameter.numberOfEntries];
        parameter.type = duplicateString(parameterHandle->type);
        parameter.name = duplicateString(parameterHandle->name);
        parameter.uniqueParameterId = parameterHandle->paramId;

        int j = 0;
        for (auto& e : parameterMap) {
            parameter.parameterMap[j].key = e.first.c_str();
            parameter.parameterMap[j].value = &e.second[0];
            parameter.parameterMap[j].numberOfValues = e.second.size();
            ++j;
        }
    }

    instanceHandle->describeInContextList = request->list;

    return globalUniquePluginIndex;
}

int renderImage(RenderImageRequest* imageRequest)
{
    PluginDefinition* pluginDefinition = createdPlugins[imageRequest->pluginIndex];
    OfxImageEffectHandle effectHandle = pluginDefinition->effectHandle;

    Image* sourceImage = new Image(imageRequest->width, imageRequest->height, imageRequest->inputImage);

    CurrentRenderRequest* renderRequest = new CurrentRenderRequest();
    renderRequest->width = imageRequest->width;
    renderRequest->height = imageRequest->height;
    renderRequest->sourceClips[kOfxImageEffectSimpleSourceClipName] = sourceImage;
    effectHandle->currentRenderRequest = renderRequest;


    OfxPropertySetHandle inParam = new OfxPropertySetStruct();
    OfxPropertySetHandle outParam = new OfxPropertySetStruct();
    propSetDouble(inParam, kOfxPropTime, 0, imageRequest->time);
    int renderWindow[4] = {0, 0, imageRequest->width, imageRequest->height};
    propSetIntN(inParam, kOfxImageEffectPropRenderWindow, 4, renderWindow);
    propSetString(inParam, kOfxImageEffectPropFieldToRender, 0, kOfxImageFieldNone);
    propSetDouble(inParam, kOfxImageEffectPropRenderScale, 0, 1.0);
    propSetDouble(inParam, kOfxImageEffectPropRenderScale, 1, 1.0);
    propSetInt(inParam, kOfxPropIsInteractive, 0, 0);

    propSetDouble(inParam, kOfxImageEffectPropFrameRange, 0, 0);
    propSetDouble(inParam, kOfxImageEffectPropFrameRange, 1, 0);
    propSetDouble(inParam, kOfxImageEffectPropFrameStep, 0, 1);

    callEntryPoint(kOfxImageEffectActionBeginSequenceRender, effectHandle, inParam, outParam, pluginDefinition->ofxPlugin);

    callEntryPoint(kOfxImageEffectActionRender, effectHandle, inParam, outParam, pluginDefinition->ofxPlugin);

    callEntryPoint(kOfxImageEffectActionEndSequenceRender, effectHandle, inParam, outParam, pluginDefinition->ofxPlugin);

    void* imageData = effectHandle->clips[kOfxImageEffectOutputClipName]->data;
    char* type = effectHandle->clips[kOfxImageEffectOutputClipName]->properties->strings["CLIP_TYPE"][0];

    bool containsByte = false;

    if (imageData != NULL) {
        int index = 0;
        char* resultImage = imageRequest->returnValue;

        if (strcmp(type, kOfxBitDepthByte) == 0) {
            char* d = (char*)imageData;
            for (int i = 0; i < imageRequest->height; ++i) {
                for (int j = 0; j < imageRequest->width; ++j) {
                    resultImage[index++] = d[i * imageRequest->width * 4 + j * 4 + 0];
                    resultImage[index++] = d[i * imageRequest->width * 4 + j * 4 + 1];
                    resultImage[index++] = d[i * imageRequest->width * 4 + j * 4 + 2];
                    resultImage[index++] = 255;
                }
            }
        } else {
            float* d = (float*)imageData;
            for (int i = 0; i < imageRequest->height; ++i) {
                for (int j = 0; j < imageRequest->width; ++j) {
                    resultImage[index++] = clamp((int)(d[i * imageRequest->width * 4 + j * 4 + 0] * 255.0f), 0, 255);
                    resultImage[index++] = clamp((int)(d[i * imageRequest->width * 4 + j * 4 + 1] * 255.0f), 0, 255);
                    resultImage[index++] = clamp((int)(d[i * imageRequest->width * 4 + j * 4 + 2] * 255.0f), 0, 255);
                    resultImage[index++] = 255;
                }
            }
        }

    }

    // delete effectHandle->currentRenderRequest->sourceClips[kOfxImageEffectSimpleSourceClipName];
    effectHandle->currentRenderRequest = NULL;
    delete inParam;
    delete outParam;

    return 0;
}

void closePlugin(int pluginIndex, int libraryIndex) {
    PluginDefinition* pluginDefinition = createdPlugins[pluginIndex];
    OfxImageEffectHandle effectHandle = pluginDefinition->effectHandle;
    callEntryPoint(kOfxActionUnload, effectHandle, NULL, NULL, pluginDefinition->ofxPlugin);

    LoadedLibraryDescriptor* libraryDescriptor = loadedLibraries[libraryIndex];

    #ifdef __linux__
        dlclose(libraryDescriptor);
    #elif _WIN32 || __CYGWIN__
        FreeLibrary((HINSTANCE)pluginDefinition->handle); 
    #endif
    createdPlugins.erase(pluginIndex);
    loadedLibraries.erase(libraryIndex);

    delete libraryDescriptor->file;
    delete libraryDescriptor;
    delete pluginDefinition;
}
}

void loadImageCallbackMock(LoadImageRequest* request) {
    Image* image = loadImage("/home/black/Downloads/image.ppm");

    request->data = (char*) image->data;
    request->width = image->width;
    request->height = image->height;
}

void deletePlugin(int pluginIndex) {
    PluginDefinition* pluginDefinition = createdPlugins[pluginIndex];
    OfxImageEffectHandle effectHandle = pluginDefinition->effectHandle;

    createdPlugins.erase(pluginIndex);

    callEntryPoint(kOfxActionDestroyInstance, effectHandle, NULL, NULL, pluginDefinition->ofxPlugin);

    deleteImageEffectHandle(effectHandle);
}

int main(int argc, char** argv) {
    InitializeHostRequest* initializeHostRequest = new InitializeHostRequest();
    initializeHostRequest->loadImageCallback = &loadImageCallbackMock;

    initializeHost(initializeHostRequest);

    LoadLibraryRequest loadLibraryRequest;
    loadLibraryRequest.file = "/home/black/tmp/openfx-misc/Misc/Linux-64-debug/Misc.ofx";
    
    int libraryIndex = loadLibrary(&loadLibraryRequest);

    LoadPluginRequest* request = new LoadPluginRequest();
    request->libraryDescriptor = libraryIndex;
    request->pluginIndex = 75;

    int width = 831;
    int height = 530;
    
    int loadedPluginIndex = loadPlugin(request);

    DescribeRequest describeRequest;
    describeRequest.pluginIndex = loadedPluginIndex;

    describe(&describeRequest);

    DescribeInContextRequest describeInContextRequest;
    describeInContextRequest.pluginIndex = loadedPluginIndex;

    describeInContext(&describeInContextRequest);

    CreateInstanceRequest createInstanceRequest;
    createInstanceRequest.width = width;
    createInstanceRequest.height = height;
    createInstanceRequest.effectId = "asd";
    createInstanceRequest.pluginIndex = loadedPluginIndex;
    int instanceIndex = createInstance(&createInstanceRequest);


    Image* sourceImage = loadImage("/home/black/Downloads/image.ppm");

    RenderImageRequest* renderImageRequest = new RenderImageRequest();
    renderImageRequest->width = sourceImage->width;
    renderImageRequest->height = sourceImage->height;
    renderImageRequest->time = 0.0;
    renderImageRequest->pluginIndex = instanceIndex;
    renderImageRequest->returnValue = new char[width * height * 4];
    renderImageRequest->inputImage = (char*)sourceImage->data;

    renderImage(renderImageRequest);

    renderImageRequest = new RenderImageRequest();
    renderImageRequest->width = sourceImage->width;
    renderImageRequest->height = sourceImage->height;
    renderImageRequest->time = 1.0;
    renderImageRequest->pluginIndex = instanceIndex;
    renderImageRequest->returnValue = new char[width * height * 4];
    renderImageRequest->inputImage = (char*)sourceImage->data;
    
    renderImage(renderImageRequest);
    Image* image = new Image(renderImageRequest->width, renderImageRequest->height, renderImageRequest->returnValue);
    writeImage("/tmp/result_native.ppm", image, "OfxBitDepthByte");


    deletePlugin(instanceIndex);
}
