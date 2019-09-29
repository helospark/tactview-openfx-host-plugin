package com.helospark.tactview.openfx;

import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.fasterxml.jackson.databind.JsonNode;
import com.helospark.tactview.core.clone.CloneRequestMetadata;
import com.helospark.tactview.core.save.LoadMetadata;
import com.helospark.tactview.core.timeline.StatelessEffect;
import com.helospark.tactview.core.timeline.TimelineInterval;
import com.helospark.tactview.core.timeline.effect.interpolation.KeyframeableEffect;
import com.helospark.tactview.core.timeline.effect.interpolation.ValueProviderDescriptor;
import com.helospark.tactview.core.timeline.effect.transition.AbstractVideoTransitionEffect;
import com.helospark.tactview.core.timeline.effect.transition.InternalStatelessVideoTransitionEffectRequest;
import com.helospark.tactview.core.timeline.image.ClipImage;
import com.helospark.tactview.openfx.nativerequest.RenderImageRequest;

public class OpenFXTransitionEffect extends AbstractVideoTransitionEffect {
    private static final String PARAMETERS_KEY = "parameters";
    private static final String PLUGIN_NAME_KEY = "pluginName";

    private OpenFxPluginInitializerResult initializedPluginData;
    private int loadedPluginIndex;
    private String pluginName;

    private OpenFxPluginInitializer openFxPluginInitializer;

    public OpenFXTransitionEffect(TimelineInterval interval, int loadedPluginIndex, OpenFxPluginInitializer openFxPluginInitializer, String pluginName) {
        super(interval);
        initializedPluginData = openFxPluginInitializer.initialize(loadedPluginIndex, getId(), Collections.emptyMap());
        this.loadedPluginIndex = loadedPluginIndex;
        this.pluginName = pluginName;
    }

    public OpenFXTransitionEffect(OpenFXTransitionEffect openfxEffect, CloneRequestMetadata cloneRequestMetadata) {
        super(openfxEffect, cloneRequestMetadata);
        this.openFxPluginInitializer = openfxEffect.openFxPluginInitializer;
        this.loadedPluginIndex = openfxEffect.loadedPluginIndex;

        Map<String, KeyframeableEffect> clonedParameterMap = new HashMap<>();
        for (var entry : openfxEffect.initializedPluginData.nameToParameter.entrySet()) {
            clonedParameterMap.put(entry.getKey(), entry.getValue().deepClone());
        }

        this.initializedPluginData = openFxPluginInitializer.initialize(openfxEffect.loadedPluginIndex, getId(), clonedParameterMap);
        this.pluginName = openfxEffect.pluginName;

    }

    public OpenFXTransitionEffect(JsonNode node, LoadMetadata loadMetadata, OpenFxPluginInitializer openFxPluginInitializer,
            Map<String, Integer> pluginNameToLoadedPluginId) {
        super(node, loadMetadata);

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
    protected void generateSavedContentInternal(Map<String, Object> result) {
        super.generateSavedContentInternal(result);
        result.put(PLUGIN_NAME_KEY, pluginName);
        result.put(PARAMETERS_KEY, initializedPluginData.nameToParameter);
    }

    @Override
    protected ClipImage applyTransitionInternal(InternalStatelessVideoTransitionEffectRequest transitionRequest) {
        synchronized (this) {
            int width = transitionRequest.getFirstFrame().getWidth();
            int height = transitionRequest.getFirstFrame().getHeight();

            ClipImage result = ClipImage.sameSizeAs(transitionRequest.getFirstFrame());

            RenderImageRequest renderImageRequest = new RenderImageRequest();
            renderImageRequest.width = width;
            renderImageRequest.height = height;
            renderImageRequest.time = transitionRequest.getEffectPosition().getSeconds().doubleValue();
            renderImageRequest.pluginIndex = initializedPluginData.createdInstanceIndex;
            renderImageRequest.returnValue = result.getBuffer();
            renderImageRequest.inputImage = transitionRequest.getFirstFrame().getBuffer();
            renderImageRequest.effectId = getId();

            renderImageRequest.isTransition = 1;
            renderImageRequest.transitionProgress = transitionRequest.getProgress();
            renderImageRequest.transitionInputImage = transitionRequest.getSecondFrame().getBuffer();

            OpenfxLibrary.INSTANCE.renderImage(renderImageRequest);

            return result;
        }
    }

    @Override
    public void preDestroy() {
        OpenfxLibrary.INSTANCE.deletePlugin(initializedPluginData.createdInstanceIndex);
    }

    @Override
    public List<ValueProviderDescriptor> getValueProviders() {
        List<ValueProviderDescriptor> valueProviders = super.getValueProviders();
        valueProviders.addAll(initializedPluginData.descriptors);
        return valueProviders;
    }

    @Override
    public StatelessEffect cloneEffect(CloneRequestMetadata cloneRequestMetadata) {
        return new OpenFXTransitionEffect(this, cloneRequestMetadata);
    }

    public Map<Integer, KeyframeableEffect> getProviders() {
        return initializedPluginData.providers;
    }

}
