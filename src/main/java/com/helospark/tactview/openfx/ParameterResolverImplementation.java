package com.helospark.tactview.openfx;

import java.util.Optional;

import com.helospark.lightdi.annotation.Component;
import com.helospark.tactview.core.repository.ProjectRepository;
import com.helospark.tactview.core.timeline.TimelinePosition;
import com.helospark.tactview.core.timeline.effect.interpolation.KeyframeableEffect;
import com.helospark.tactview.core.timeline.effect.interpolation.pojo.Color;
import com.helospark.tactview.core.timeline.effect.interpolation.pojo.Point;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.BooleanProvider;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.DoubleProvider;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.IntegerProvider;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.PointProvider;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.ValueListProvider;
import com.helospark.tactview.openfx.OpenFxToTactviewParameterMapper.OpenfxValueListElement;
import com.helospark.tactview.openfx.nativerequest.ParameterValueProviderCallback;
import com.helospark.tactview.openfx.nativerequest.ResolveValueRequest;

@Component
public class ParameterResolverImplementation implements ParameterValueProviderCallback {
    private OpenfxParameterRepository openfxParameterRepository;
    private ProjectRepository projectRepository;

    public ParameterResolverImplementation(OpenfxParameterRepository openfxParameterRepository, ProjectRepository projectRepository) {
        this.openfxParameterRepository = openfxParameterRepository;
        this.projectRepository = projectRepository;
    }

    @Override
    public void resolveValue(ResolveValueRequest resolveValueRequest) {

        Optional<KeyframeableEffect> optionalParameter = openfxParameterRepository.findKeyframeableEffect(resolveValueRequest.uniqueId);

        if (!optionalParameter.isPresent()) {
            System.out.println("Asking for nonexistent parameter");
            return;
        }
        KeyframeableEffect keyframeableEffect = optionalParameter.get();

        TimelinePosition position = new TimelinePosition(resolveValueRequest.time);
        if (keyframeableEffect instanceof IntegerProvider) {
            resolveValueRequest.result.intValue1 = ((IntegerProvider) keyframeableEffect).getValueWithoutScriptAt(position);
        } else if (keyframeableEffect instanceof BooleanProvider) {
            resolveValueRequest.result.intValue1 = ((BooleanProvider) keyframeableEffect).getValueWithoutScriptAt(position) ? 1 : 0;
        } else if (keyframeableEffect instanceof DoubleProvider) {
            resolveValueRequest.result.doubleValue1 = ((DoubleProvider) keyframeableEffect).getValueWithoutScriptAt(position);
        } else if (keyframeableEffect instanceof ResultMappableColorProvider) {
            Color color = ((ResultMappableColorProvider) keyframeableEffect).getUnmappedValue(position);
            resolveValueRequest.result.doubleValue1 = color.red;
            resolveValueRequest.result.doubleValue2 = color.green;
            resolveValueRequest.result.doubleValue3 = color.blue;
            resolveValueRequest.result.doubleValue4 = 1.0; // alpha always 1.0 for now
        } else if (keyframeableEffect instanceof ValueListProvider) {
            OpenfxValueListElement element = (OpenfxValueListElement) ((ValueListProvider) keyframeableEffect).getValueWithoutScriptAt(position);
            resolveValueRequest.result.intValue1 = element.openfxId;
        } else if (keyframeableEffect instanceof PointProvider) {
            PointProvider pointProvider = (PointProvider) keyframeableEffect;
            Point point = pointProvider.getValueWithoutScriptAt(position);
            resolveValueRequest.result.doubleValue1 = point.x;
            resolveValueRequest.result.doubleValue2 = point.y;
        } else {
            System.out.println("No mapping for parameter " + keyframeableEffect.getId());
        }

    }

}
