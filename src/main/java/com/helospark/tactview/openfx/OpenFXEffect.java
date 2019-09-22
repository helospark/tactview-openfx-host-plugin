package com.helospark.tactview.openfx;

import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.List;

import com.fasterxml.jackson.databind.JsonNode;
import com.helospark.tactview.core.clone.CloneRequestMetadata;
import com.helospark.tactview.core.decoder.framecache.GlobalMemoryManagerAccessor;
import com.helospark.tactview.core.save.LoadMetadata;
import com.helospark.tactview.core.timeline.StatelessEffect;
import com.helospark.tactview.core.timeline.StatelessVideoEffect;
import com.helospark.tactview.core.timeline.TimelineInterval;
import com.helospark.tactview.core.timeline.effect.StatelessEffectRequest;
import com.helospark.tactview.core.timeline.effect.interpolation.ValueProviderDescriptor;
import com.helospark.tactview.core.timeline.image.ClipImage;
import com.helospark.tactview.core.timeline.image.ReadOnlyClipImage;
import com.helospark.tactview.core.util.ReflectionUtil;
import com.helospark.tactview.openfx.nativerequest.RenderImageRequest;

public class OpenFXEffect extends StatelessVideoEffect {
    private int pluginIndex;

    public OpenFXEffect(TimelineInterval interval, int pluginIndex) {
        super(interval);
        this.pluginIndex = pluginIndex;
    }

    public OpenFXEffect(OpenFXEffect openfxEffect, CloneRequestMetadata cloneRequestMetadata) {
        super(openfxEffect, cloneRequestMetadata);
        ReflectionUtil.copyOrCloneFieldFromTo(openfxEffect, this);
        // pluginIndex
    }

    public OpenFXEffect(JsonNode node, LoadMetadata loadMetadata) {
        super(node, loadMetadata);
        // pluginIndex
    }

    @Override
    public ReadOnlyClipImage createFrame(StatelessEffectRequest request) {
        synchronized (this) {
            int width = request.getCurrentFrame().getWidth();
            int height = request.getCurrentFrame().getHeight();

            ByteBuffer result = GlobalMemoryManagerAccessor.memoryManager.requestBuffer(width * height * 4);

            RenderImageRequest renderImageRequest = new RenderImageRequest();
            renderImageRequest.width = width;
            renderImageRequest.height = height;
            renderImageRequest.time = request.getEffectPosition().getSeconds().doubleValue();
            renderImageRequest.pluginIndex = pluginIndex;
            renderImageRequest.returnValue = result;
            renderImageRequest.inputImage = request.getCurrentFrame().getBuffer();

            OpenfxLibrary.INSTANCE.renderImage(renderImageRequest);

            ClipImage clipImage = new ClipImage(result, width, height);
            return clipImage;
        }
    }

    @Override
    public void initializeValueProvider() {
    }

    @Override
    public List<ValueProviderDescriptor> getValueProviders() {
        return Arrays.asList();
    }

    @Override
    public StatelessEffect cloneEffect(CloneRequestMetadata cloneRequestMetadata) {
        return new OpenFXEffect(this, cloneRequestMetadata);
    }

}
