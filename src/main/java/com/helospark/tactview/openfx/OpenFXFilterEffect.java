package com.helospark.tactview.openfx;

import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.fasterxml.jackson.databind.JsonNode;
import com.helospark.tactview.core.clone.CloneRequestMetadata;
import com.helospark.tactview.core.save.LoadMetadata;
import com.helospark.tactview.core.timeline.StatelessEffect;
import com.helospark.tactview.core.timeline.StatelessVideoEffect;
import com.helospark.tactview.core.timeline.TimelineInterval;
import com.helospark.tactview.core.timeline.TimelinePosition;
import com.helospark.tactview.core.timeline.effect.StatelessEffectRequest;
import com.helospark.tactview.core.timeline.effect.interpolation.KeyframeableEffect;
import com.helospark.tactview.core.timeline.effect.interpolation.ValueProviderDescriptor;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.DependentClipProvider;
import com.helospark.tactview.core.timeline.image.ClipImage;
import com.helospark.tactview.core.timeline.image.ReadOnlyClipImage;
import com.helospark.tactview.openfx.nativerequest.RenderImageClip;
import com.helospark.tactview.openfx.nativerequest.RenderImageRequest;

public class OpenFXFilterEffect extends StatelessVideoEffect {
    private static final String PARAMETERS_KEY = "parameters";

    private static final String PLUGIN_NAME_KEY = "pluginName";

    private OpenFxPluginInitializerResult initializedPluginData;
    private int loadedPluginIndex;
    private String pluginName;

    private OpenFxPluginInitializer openFxPluginInitializer;

    public OpenFXFilterEffect(TimelineInterval interval, int loadedPluginIndex, OpenFxPluginInitializer openFxPluginInitializer, String pluginName) {
        super(interval);
        initializedPluginData = openFxPluginInitializer.initialize(loadedPluginIndex, getId(), Collections.emptyMap());
        this.loadedPluginIndex = loadedPluginIndex;
        this.pluginName = pluginName;
    }

    public OpenFXFilterEffect(OpenFXFilterEffect openfxEffect, CloneRequestMetadata cloneRequestMetadata) {
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

    public OpenFXFilterEffect(JsonNode node, LoadMetadata loadMetadata, OpenFxPluginInitializer openFxPluginInitializer,
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
    public void preDestroy() {
        OpenfxLibrary.INSTANCE.deletePlugin(initializedPluginData.createdInstanceIndex);
    }

    @Override
    public ReadOnlyClipImage createFrame(StatelessEffectRequest request) {
        synchronized (this) {
            int width = request.getCurrentFrame().getWidth();
            int height = request.getCurrentFrame().getHeight();

            Map<String, ReadOnlyClipImage> requestedClips = request.getRequestedClips();

            Map<String, ReadOnlyClipImage> clipNameToClipImage = new HashMap<>();

            for (var entry : initializedPluginData.nameToParameter.entrySet()) {
                if (entry.getValue() instanceof DependentClipProvider) {
                    ReadOnlyClipImage clipImage = requestedClips.get(((DependentClipProvider) entry.getValue()).getValueAt(request.getEffectPosition()));
                    if (clipImage != null) {
                        clipNameToClipImage.put(entry.getKey(), clipImage);
                    }
                }
            }

            ClipImage result = ClipImage.sameSizeAs(request.getCurrentFrame());

            RenderImageRequest renderImageRequest = new RenderImageRequest();
            renderImageRequest.width = width;
            renderImageRequest.height = height;
            renderImageRequest.time = request.getEffectPosition().getSeconds().doubleValue();
            renderImageRequest.pluginIndex = initializedPluginData.createdInstanceIndex;
            renderImageRequest.returnValue = result.getBuffer();
            renderImageRequest.inputImage = request.getCurrentFrame().getBuffer();
            renderImageRequest.effectId = getId();
            renderImageRequest.numberOfAdditionalClips = clipNameToClipImage.size();

            if (renderImageRequest.numberOfAdditionalClips > 0) {
                renderImageRequest.clips = new RenderImageClip();
                RenderImageClip[] elements = (RenderImageClip[]) renderImageRequest.clips.toArray(renderImageRequest.numberOfAdditionalClips);
                int i = 0;
                for (var entry : clipNameToClipImage.entrySet()) {
                    elements[i].width = entry.getValue().getWidth();
                    elements[i].height = entry.getValue().getHeight();
                    elements[i].data = entry.getValue().getBuffer();
                    elements[i].name = entry.getKey();
                    ++i;
                }
            }

            OpenfxLibrary.INSTANCE.renderImage(renderImageRequest);

            return result;
        }
    }

    @Override
    public void initializeValueProvider() {
    }

    @Override
    public List<ValueProviderDescriptor> getValueProviders() {
        return initializedPluginData.descriptors;
    }

    @Override
    public StatelessEffect cloneEffect(CloneRequestMetadata cloneRequestMetadata) {
        return new OpenFXFilterEffect(this, cloneRequestMetadata);
    }

    public Map<Integer, KeyframeableEffect> getProviders() {
        return initializedPluginData.providers;
    }

    @Override
    public List<String> getClipDependency(TimelinePosition position) {
        List<String> dependencies = super.getClipDependency(position);
        initializedPluginData.nameToParameter.values()
                .stream()
                .filter(a -> a instanceof DependentClipProvider)
                .map(a -> ((DependentClipProvider) a).getValueAt(position))
                .filter(a -> a != null)
                .filter(a -> !a.isEmpty())
                .forEach(a -> dependencies.add(a));
        return dependencies;
    }
}
