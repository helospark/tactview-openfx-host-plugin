package com.helospark.tactview.openfx;

import java.util.Map;

import com.fasterxml.jackson.databind.JsonNode;
import com.helospark.tactview.core.save.LoadMetadata;
import com.helospark.tactview.core.timeline.effect.interpolation.AbstractKeyframeableEffectDesSerFactory;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.ColorProviderFactory;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.DoubleProvider;
import com.helospark.tactview.core.util.ReflectionUtil;

public class ResultMappableColorProviderFactory extends AbstractKeyframeableEffectDesSerFactory<ResultMappableColorProvider> {
    ColorProviderFactory colorProviderFactory = new ColorProviderFactory();

    @Override
    public void addDataForDeserializeInternal(ResultMappableColorProvider instance, Map<String, Object> data) {
        colorProviderFactory.addDataForDeserializeInternal(instance, data);
    }

    @Override
    public ResultMappableColorProvider deserializeInternal(JsonNode data, ResultMappableColorProvider currentFieldValue, LoadMetadata loadMetadata) {
        return new ResultMappableColorProvider(ReflectionUtil.deserialize(data.get("redProvider"), DoubleProvider.class, currentFieldValue.getRedProvider(), loadMetadata),
                ReflectionUtil.deserialize(data.get("greenProvider"), DoubleProvider.class, currentFieldValue.getGreenProvider(), loadMetadata),
                ReflectionUtil.deserialize(data.get("blueProvider"), DoubleProvider.class, currentFieldValue.getBlueProvider(), loadMetadata),
                currentFieldValue.min, currentFieldValue.max);
    }
}
