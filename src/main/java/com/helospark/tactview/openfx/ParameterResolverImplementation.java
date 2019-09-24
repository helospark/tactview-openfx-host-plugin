package com.helospark.tactview.openfx;

import com.helospark.lightdi.annotation.Component;
import com.helospark.tactview.core.timeline.TimelinePosition;
import com.helospark.tactview.core.timeline.effect.interpolation.KeyframeableEffect;
import com.helospark.tactview.core.timeline.effect.interpolation.pojo.Color;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.BooleanProvider;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.DoubleProvider;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.IntegerProvider;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.ValueListProvider;
import com.helospark.tactview.openfx.OpenFXEffect.OpenfxValueListElement;
import com.helospark.tactview.openfx.nativerequest.ParameterValueProviderCallback;
import com.helospark.tactview.openfx.nativerequest.ResolveValueRequest;

@Component
public class ParameterResolverImplementation implements ParameterValueProviderCallback {
    private OpenfxParameterRepository openfxParameterRepository;

    public ParameterResolverImplementation(OpenfxParameterRepository openfxParameterRepository) {
        this.openfxParameterRepository = openfxParameterRepository;
    }

    @Override
    public void resolveValue(ResolveValueRequest resolveValueRequest) {

        KeyframeableEffect keyframeableEffect = openfxParameterRepository.findKeyframeableEffect(resolveValueRequest.uniqueId).get();

        if (keyframeableEffect instanceof IntegerProvider) {
            resolveValueRequest.result.intValue1 = ((IntegerProvider) keyframeableEffect).getValueAt(new TimelinePosition(resolveValueRequest.time));
        } else if (keyframeableEffect instanceof BooleanProvider) {
            resolveValueRequest.result.intValue1 = ((BooleanProvider) keyframeableEffect).getValueAt(new TimelinePosition(resolveValueRequest.time)) ? 1 : 0;
        } else if (keyframeableEffect instanceof DoubleProvider) {
            resolveValueRequest.result.doubleValue1 = ((DoubleProvider) keyframeableEffect).getValueAt(new TimelinePosition(resolveValueRequest.time));
        } else if (keyframeableEffect instanceof ResultMappableColorProvider) {
            Color color = ((ResultMappableColorProvider) keyframeableEffect).getUnmappedValue(new TimelinePosition(resolveValueRequest.time));
            resolveValueRequest.result.doubleValue1 = color.red;
            resolveValueRequest.result.doubleValue2 = color.green;
            resolveValueRequest.result.doubleValue3 = color.blue;
        } else if (keyframeableEffect instanceof ValueListProvider) {
            OpenfxValueListElement element = (OpenfxValueListElement) ((ValueListProvider) keyframeableEffect).getValueAt(new TimelinePosition(resolveValueRequest.time));
            resolveValueRequest.result.intValue1 = element.openfxId;
        }

    }

}
