#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <ofxCore.h>
#include <iostream>
#include "imageEffectStruct.h"
#include "ofx_property.h"
#include "paramSet.h"
#include "imageLoader.h"
#include "multithread.h"
#include "memorySuite.h"
#include "messageSuite.h"

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

    std::cout << " :( missing feature" << std::endl;

    return (void*)0;
}

int main(int argc, char** argv)
{
    void *handle;

    //handle = dlopen("/home/black/Desktop/Examples/Basic/Linux-64-debug/basic.ofx", RTLD_LAZY);
    //handle = dlopen("/usr/local/Neat Video v5 OFX/NeatVideo5.ofx.bundle/Contents/Linux-x86-64/NeatVideo5.ofx", RTLD_LAZY);
    handle = dlopen("/home/black/Downloads/tuttle-v0.8-1-g9daa09f (2)/plugin/Ramp-2.0.ofx.bundle/Contents/Linux-x86-64/Ramp-2.0.ofx", RTLD_LAZY);
    //handle = dlopen("/home/black/Downloads/tuttle-v0.8-1-g9daa09f (2)/plugin/ColorBars-2.0.ofx.bundle/Contents/Linux-x86-64/ColorBars-2.0.ofx", RTLD_LAZY);

    if (!handle) {
        /* fail to load the library */
        fprintf(stderr, "Error: %s\n", dlerror());
        return EXIT_FAILURE;
    }

    intFptr func_print_name = (intFptr) dlsym(handle, "OfxGetNumberOfPlugins");
    if (!func_print_name) {
        /* no such symbol */
        fprintf(stderr, "Error: %s\n", dlerror());
        dlclose(handle);
        return EXIT_FAILURE;
    }
    int plugins = func_print_name();
    fprintf(stderr, "Loaded, number of plugins %d\n", plugins);


    getPluginFptr getPlugin = (getPluginFptr) dlsym(handle, "OfxGetPlugin");

    OfxPlugin* plugin = getPlugin(0);

    std::cout << "Api version = " << plugin->apiVersion << std::endl;

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
    propSetInt(host->host, kOfxImageEffectPropTemporalClipAccess, 0, 0);

    const char* supportedComponents[] = {"kOfxImageComponentRGBA", "kOfxImageComponentAlpha"};
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
    const char* supportedPixelDepths[] = {kOfxBitDepthByte};
    propSetStringN(host->host, kOfxImageEffectPropSupportedPixelDepths, 1, supportedPixelDepths);
    int rowColumnCount[2] = {10,1};
    propSetIntN(host->host, kOfxParamHostPropPageRowColumnCount, 2, rowColumnCount);
    propSetPointer(host->host, kOfxPropHostOSHandle, 0, NULL);
    propSetInt(host->host, kOfxImageEffectInstancePropSequentialRender, 0, 0);
    //propSetInt(host->host, kOfxImageEffectPropOpenGLRenderSupported, 0, 0);
    propSetInt(host->host, kOfxImageEffectPropRenderQualityDraft, 0, 1);
    //propSetString(host->host, kOfxImageEffectHostPropNativeOrigin, 0, kOfxImageEffectHostPropNativeOriginTopLeft);
    
    propSetString(host->host, kOfxPropVersion, 0, "1.0");
    



    plugin->setHost(host);

    OfxImageEffectHandle effectHandle = new OfxImageEffectStruct();
    propSetString(effectHandle->properties, kOfxPropType, 0, kOfxTypeImageEffect);
    propSetString(effectHandle->properties, kOfxPropLabel, 0, "label");
    propSetString(effectHandle->properties, kOfxPropShortLabel, 0, "label");
    propSetString(effectHandle->properties, kOfxPropLongLabel, 0, "label");
    propSetString(effectHandle->properties, kOfxPropVersion, 0, "1");
    propSetString(effectHandle->properties, kOfxPropVersionLabel, 0, "1");
    propSetString(effectHandle->properties, kOfxPluginPropFilePath, 0, "/tmp");
    propSetString(effectHandle->properties, kOfxImageEffectPropSupportedContexts, 0, kOfxImageEffectContextFilter);
    propSetString(effectHandle->properties, kOfxImageEffectPropSupportedPixelDepths, 0, kOfxBitDepthByte);



    OfxPropertySetHandle inParam = new OfxPropertySetStruct();
    OfxPropertySetHandle outParam = new OfxPropertySetStruct();

    std::cout << "Loading plugin" << std::endl;
    plugin->mainEntry(kOfxActionLoad, effectHandle, inParam, outParam);
    
    std::cout << "Calling entrypoint" << std::endl;

    plugin->mainEntry(kOfxActionDescribe, effectHandle, inParam, outParam);
    
    std::cout << "Returning back" << std::endl;

    printAllProperties(outParam);
    printAllProperties(effectHandle->properties);


    
    propSetString(effectHandle->properties, kOfxImageEffectPropContext, 0, kOfxImageEffectContextGenerator);
    propSetString(effectHandle->properties, kOfxImageEffectPropComponents, 0, kOfxImageComponentRGB);
    propSetString(inParam, kOfxImageEffectPropContext, 0, kOfxImageEffectContextFilter);

    std::cout << "Calling describeInContext" << std::endl;

    plugin->mainEntry(kOfxImageEffectActionDescribeInContext, effectHandle, inParam, outParam);

    printAllProperties(effectHandle->properties);
    printAllProperties(effectHandle->clips[kOfxImageEffectSimpleSourceClipName]->properties);
    printAllProperties(effectHandle->clips[kOfxImageEffectOutputClipName]->properties);
    for (int i = 0; i < effectHandle->parameters->parameters.size(); ++i) {
        printAllProperties(effectHandle->parameters->parameters[i]->properties);
    }

    plugin->mainEntry(kOfxActionCreateInstance, effectHandle, inParam, outParam);

    fprintf(stderr, "Plugin apiVersion=%s version=%d, pluginIdentifier=%s, pluginVersionMajor=%d, pluginVersionMinor=%d\n", plugin->pluginApi, plugin->apiVersion, plugin->pluginIdentifier, plugin->pluginVersionMajor, plugin->pluginVersionMinor);

    propSetDouble(inParam, kOfxPropTime, 0, 0.0);
    int renderWindow[4] = {0, 0, 831, 530};
    propSetIntN(inParam, kOfxImageEffectPropRenderWindow, 4, renderWindow);
    propSetString(inParam, kOfxImageEffectPropFieldToRender, 0, kOfxImageFieldNone);
    propSetDouble(inParam, kOfxImageEffectPropRenderScale, 0, 1.0);
    propSetDouble(inParam, kOfxImageEffectPropRenderScale, 1, 1.0);
    propSetInt(inParam, kOfxPropIsInteractive, 0, 0);

    propSetDouble(inParam, kOfxImageEffectPropFrameRange, 0, 0);
    propSetDouble(inParam, kOfxImageEffectPropFrameRange, 1, 0);
    propSetDouble(inParam, kOfxImageEffectPropFrameStep, 0, 1);

    plugin->mainEntry(kOfxImageEffectActionBeginSequenceRender, effectHandle, inParam, outParam);

    plugin->mainEntry(kOfxImageEffectActionRender, effectHandle, inParam, outParam);

    plugin->mainEntry(kOfxImageEffectActionEndSequenceRender, effectHandle, inParam, outParam);

    char* imageData = effectHandle->clips[kOfxImageEffectOutputClipName]->data;


    Image result(831, 530, imageData);
    writeImage("/home/black/Downloads/image3.ppm", &result);

    dlclose(handle);

    return 0;
}
