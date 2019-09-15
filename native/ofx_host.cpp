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

OfxPlugin* plugin;

void callEntryPoint(const char *action, const void *handle, OfxPropertySetHandle inArgs, OfxPropertySetHandle outArgs) {
    std::cout << "Calling " << action << std::endl;
    OfxStatus result = plugin->mainEntry(action, handle, inArgs, outArgs);   
    std::cout << action << " done" << std::endl;
    if (result != kOfxStatOK) {
        std::cout << "Error, result is " << result << std::endl;
    }

    if (result == kOfxStatErrUnsupported) {
        exit(1);
    }
}

struct PluginDefinition {
    OfxImageEffectHandle effectHandle;
    void* handle;
    int pluginIndex;
};

std::map<int, PluginDefinition*> loadedPlugins;
int pluginIndex = 0;
std::map<std::string, void*> handles;

std::vector<PluginDefinition> loadDefinitions() {

}

struct LoadPluginRequest {
    int width;
    int height;
    OfxHost* host;
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

int loadPlugin(LoadPluginRequest* loadPluginRequest) {

    void *handle;
    
    const char* file = "/home/black/tmp/openfx-misc/Misc/Linux-64-debug/Misc.ofx"; 

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


    plugin = getPlugin(k);

    std::cout << "Api version = " << plugin->apiVersion << " plugin_index=" << k << std::endl;

    plugin->setHost(loadPluginRequest->host);

    OfxImageEffectHandle effectHandle = new OfxImageEffectStruct();
    propSetString(effectHandle->properties, kOfxPropType, 0, kOfxTypeImageEffect);
    propSetString(effectHandle->properties, kOfxPropLabel, 0, "label");
    propSetString(effectHandle->properties, kOfxPropShortLabel, 0, "label");
    propSetString(effectHandle->properties, kOfxPropLongLabel, 0, "label");
    propSetString(effectHandle->properties, kOfxPropVersion, 0, "1");
    propSetString(effectHandle->properties, kOfxPropVersionLabel, 0, "1");
    propSetString(effectHandle->properties, kOfxPluginPropFilePath, 0, file);
    propSetString(effectHandle->properties, kOfxImageEffectPropSupportedContexts, 0, kOfxImageEffectContextGeneral);
    propSetString(effectHandle->properties, kOfxImageEffectPropSupportedPixelDepths, 0, kOfxBitDepthByte);

    propSetDouble(effectHandle->properties, kOfxImageEffectPropProjectExtent, 0, loadPluginRequest->width);
    propSetDouble(effectHandle->properties, kOfxImageEffectPropProjectExtent, 1, loadPluginRequest->height);

    propSetDouble(effectHandle->properties, kOfxImageEffectPropProjectSize, 0, loadPluginRequest->width);
    propSetDouble(effectHandle->properties, kOfxImageEffectPropProjectSize, 1, loadPluginRequest->height);

    propSetDouble(effectHandle->properties, kOfxImageEffectPropProjectOffset, 0, 0);
    propSetDouble(effectHandle->properties, kOfxImageEffectPropProjectOffset, 1, 0);


    OfxPropertySetHandle inParam = new OfxPropertySetStruct();
    OfxPropertySetHandle outParam = new OfxPropertySetStruct();

    callEntryPoint(kOfxActionLoad, effectHandle, inParam, outParam);

    std::cout << "Load outParam params:" << std::endl;
    printAllProperties(outParam);
    
    PluginDefinition* pluginDefinition = new PluginDefinition();
    pluginDefinition->effectHandle = effectHandle;
    pluginDefinition->handle = handle;
    pluginDefinition->pluginIndex = k;

    pluginIndex++;
    loadedPlugins[pluginIndex] = pluginDefinition;

    delete inParam;
    delete outParam;

    return pluginIndex;
}

struct RenderImageRequest {
    int width;
    int height;
    int pluginIndex;
    double time;
};

int renderImage(RenderImageRequest* imageRequest)
{
    PluginDefinition* pluginDefinition = loadedPlugins[pluginIndex];
    OfxImageEffectHandle effectHandle = pluginDefinition->effectHandle;
    std::cout << "Calling entrypoint" << std::endl;


    OfxPropertySetHandle inParam = new OfxPropertySetStruct();
    OfxPropertySetHandle outParam = new OfxPropertySetStruct();

    callEntryPoint(kOfxActionDescribe, effectHandle, inParam, outParam);
    
    std::cout << "Returning back" << std::endl;

    printAllProperties(outParam);
    printAllProperties(effectHandle->properties);


    
    propSetString(effectHandle->properties, kOfxImageEffectPropContext, 0, kOfxImageEffectContextFilter);
    propSetString(effectHandle->properties, kOfxImageEffectPropComponents, 0, kOfxImageComponentRGB);
    propSetString(inParam, kOfxImageEffectPropContext, 0, kOfxImageEffectContextFilter);

    std::cout << "Calling describeInContext" << std::endl;

    callEntryPoint(kOfxImageEffectActionDescribeInContext, effectHandle, inParam, outParam);

    printAllProperties(effectHandle->properties);
  //  printAllProperties(effectHandle->clips[kOfxImageEffectSimpleSourceClipName]->properties);
  //  printAllProperties(effectHandle->clips[kOfxImageEffectOutputClipName]->properties);
    for (int i = 0; i < effectHandle->parameters->parameters.size(); ++i) {
        printAllProperties(effectHandle->parameters->parameters[i]->properties);
    }

    callEntryPoint(kOfxActionCreateInstance, effectHandle, inParam, outParam);

    fprintf(stderr, "Plugin apiVersion=%s version=%d, pluginIdentifier=%s, pluginVersionMajor=%d, pluginVersionMinor=%d\n", plugin->pluginApi, plugin->apiVersion, plugin->pluginIdentifier, plugin->pluginVersionMajor, plugin->pluginVersionMinor);

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

    callEntryPoint(kOfxImageEffectActionBeginSequenceRender, effectHandle, inParam, outParam);

    callEntryPoint(kOfxImageEffectActionRender, effectHandle, inParam, outParam);

    callEntryPoint(kOfxImageEffectActionEndSequenceRender, effectHandle, inParam, outParam);

    void* imageData = effectHandle->clips[kOfxImageEffectOutputClipName]->data;
    char* type = effectHandle->clips[kOfxImageEffectOutputClipName]->properties->strings["CLIP_TYPE"][0];

    bool containsByte = false;

    if (imageData != NULL) {
        std::cout << "Rendering result" << std::endl;
        Image result(imageRequest->width, imageRequest->height, imageData);
        std::stringstream ss("");
        char* label = new char[200];
        propGetString(effectHandle->properties, kOfxPropLabel, 0, &label);
        ss << "/tmp/image_" << pluginDefinition->pluginIndex << "_" << label << ".ppm";
        std::cout << "Writing file " << ss.str().c_str() << std::endl;
        writeImage(ss.str().c_str(), &result, type);
    }

    return 0;
}

void closePlugin(int pluginIndex) {
    PluginDefinition* pluginDefinition = loadedPlugins[pluginIndex];
    #ifdef __linux__
        dlclose(pluginDefinition->handle);
    #elif _WIN32 || __CYGWIN__
        FreeLibrary((HINSTANCE)pluginDefinition->handle); 
    #endif
    loadedPlugins.erase(pluginIndex);
}

int main(int argc, char** argv) {
    if (globalHost == NULL) {
        globalHost = createGlobalHost();
    }

    LoadPluginRequest* request = new LoadPluginRequest();
    request->width = 831;
    request->height = 530;
    request->host = globalHost;
    
    int pluginIndex = loadPlugin(request);

    RenderImageRequest* renderImageRequest = new RenderImageRequest();
    renderImageRequest->width = request->width;
    renderImageRequest->height = request->height;
    renderImageRequest->time = 0.0;
    renderImageRequest->pluginIndex = pluginIndex;

    renderImage(renderImageRequest);

    closePlugin(pluginIndex);
}
