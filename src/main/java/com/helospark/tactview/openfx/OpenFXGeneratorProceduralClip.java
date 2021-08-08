package com.helospark.tactview.openfx;

import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.fasterxml.jackson.databind.JsonNode;
import com.helospark.tactview.core.clone.CloneRequestMetadata;
import com.helospark.tactview.core.decoder.ImageMetadata;
import com.helospark.tactview.core.decoder.VisualMediaMetadata;
import com.helospark.tactview.core.save.LoadMetadata;
import com.helospark.tactview.core.save.SaveMetadata;
import com.helospark.tactview.core.timeline.GetFrameRequest;
import com.helospark.tactview.core.timeline.TimelineClip;
import com.helospark.tactview.core.timeline.TimelineInterval;
import com.helospark.tactview.core.timeline.TimelinePosition;
import com.helospark.tactview.core.timeline.effect.interpolation.KeyframeableEffect;
import com.helospark.tactview.core.timeline.effect.interpolation.ValueProviderDescriptor;
import com.helospark.tactview.core.timeline.image.ClipImage;
import com.helospark.tactview.core.timeline.image.ReadOnlyClipImage;
import com.helospark.tactview.core.timeline.proceduralclip.ProceduralVisualClip;
import com.helospark.tactview.openfx.nativerequest.RenderImageRequest;

public class OpenFXGeneratorProceduralClip extends ProceduralVisualClip {
    private static final String PARAMETERS_KEY = "parameters";
    private static final String PLUGIN_NAME_KEY = "pluginName";

    private OpenFxPluginInitializerResult initializedPluginData;
    private int loadedPluginIndex;
    private String pluginName;

    private OpenFxPluginInitializer openFxPluginInitializer;

    public OpenFXGeneratorProceduralClip(VisualMediaMetadata visualMediaMetadata, TimelineInterval interval, int loadedPluginIndex, OpenFxPluginInitializer openFxPluginInitializer,
            String pluginName) {
        super(visualMediaMetadata, interval);
        initializedPluginData = openFxPluginInitializer.initialize(loadedPluginIndex, getId(), Collections.emptyMap());
        this.loadedPluginIndex = loadedPluginIndex;
        this.pluginName = pluginName;
    }

    public OpenFXGeneratorProceduralClip(OpenFXGeneratorProceduralClip openfxEffect, CloneRequestMetadata cloneRequestMetadata) {
        super(openfxEffect, cloneRequestMetadata);
        this.openFxPluginInitializer = openfxEffect.openFxPluginInitializer;
        this.loadedPluginIndex = openfxEffect.loadedPluginIndex;

        Map<String, KeyframeableEffect> clonedParameterMap = new HashMap<>();
        for (var entry : openfxEffect.initializedPluginData.nameToParameter.entrySet()) {
            clonedParameterMap.put(entry.getKey(), entry.getValue().deepClone(cloneRequestMetadata));
        }

        this.initializedPluginData = openFxPluginInitializer.initialize(openfxEffect.loadedPluginIndex, getId(), clonedParameterMap);
        this.pluginName = openfxEffect.pluginName;

    }

    public OpenFXGeneratorProceduralClip(ImageMetadata metadata, JsonNode node, LoadMetadata loadMetadata, OpenFxPluginInitializer openFxPluginInitializer,
            Map<String, Integer> pluginNameToLoadedPluginId) {
        super(metadata, node, loadMetadata);

        this.pluginName = node.get(PLUGIN_NAME_KEY).asText();
        Integer index = pluginNameToLoadedPluginId.get(pluginName); // hopefully name is unique :)
        if (index == null) {
            throw new RuntimeException("Plugin " + pluginName + " does not exist in this system");
        }

        JsonNode parametersNode = node.get(PARAMETERS_KEY);

        this.loadedPluginIndex = index;

        this.initializedPluginData = openFxPluginInitializer.initializePluginAfterLoad(loadedPluginIndex, getId(), parametersNode, loadMetadata);
        this.openFxPluginInitializer = openFxPluginInitializer;
    }

    @Override
    protected void generateSavedContentInternal(Map<String, Object> result, SaveMetadata saveMetadata) {
        super.generateSavedContentInternal(result, saveMetadata);
        result.put(PLUGIN_NAME_KEY, pluginName);
        result.put(PARAMETERS_KEY, initializedPluginData.nameToParameter);
    }

    @Override
    public void preDestroy() {
        OpenfxLibrary.INSTANCE.deletePlugin(initializedPluginData.createdInstanceIndex);
    }

    @Override
    public ReadOnlyClipImage createProceduralFrame(GetFrameRequest request, TimelinePosition relativePosition) {
        synchronized (this) {
            int width = request.getExpectedWidth();
            int height = request.getExpectedHeight();

            ClipImage result = ClipImage.fromSize(width, height);

            RenderImageRequest renderImageRequest = new RenderImageRequest();
            renderImageRequest.width = width;
            renderImageRequest.height = height;
            renderImageRequest.time = relativePosition.getSeconds().doubleValue();
            renderImageRequest.pluginIndex = initializedPluginData.createdInstanceIndex;
            renderImageRequest.returnValue = result.getBuffer();
            renderImageRequest.inputImage = null;
            renderImageRequest.effectId = getId();
            renderImageRequest.scale = request.getScale();

            OpenfxLibrary.INSTANCE.renderImage(renderImageRequest);

            return result;
        }

    }

    @Override
    public List<ValueProviderDescriptor> getDescriptorsInternal() {
        List<ValueProviderDescriptor> descriptors = super.getDescriptorsInternal();
        descriptors.addAll(initializedPluginData.descriptors);
        return descriptors;
    }

    @Override
    public TimelineClip cloneClip(CloneRequestMetadata cloneRequestMetadata) {
        return new OpenFXGeneratorProceduralClip(this, cloneRequestMetadata);
    }

    public Map<Integer, KeyframeableEffect> getProviders() {
        return initializedPluginData.providers;
    }

}
