package com.helospark.tactview.openfx;

import com.helospark.tactview.core.timeline.TimelinePosition;
import com.helospark.tactview.core.timeline.effect.interpolation.KeyframeableEffect;
import com.helospark.tactview.core.timeline.effect.interpolation.interpolator.MultiKeyframeBasedDoubleInterpolator;
import com.helospark.tactview.core.timeline.effect.interpolation.pojo.Color;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.ColorProvider;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.DoubleProvider;
import com.helospark.tactview.core.util.DesSerFactory;
import com.helospark.tactview.core.util.MathUtil;

public class ResultMappableColorProvider extends ColorProvider {
    Color min;
    Color max;

    public ResultMappableColorProvider(DoubleProvider redProvider, DoubleProvider greenProvider, DoubleProvider blueProvider,
            Color min, Color max) {
        super(redProvider, greenProvider, blueProvider);
        this.min = min;
        this.max = max;
    }

    @Override
    public Color getValueAt(TimelinePosition position) {
        return super.getValueAt(position);
    }

    public Color getUnmappedValue(TimelinePosition position) {
        Color c = super.getValueAt(position);
        double unMappedR = unmapComponent(c.red, min.red, max.red);
        double unMappedG = unmapComponent(c.green, min.green, max.green);
        double unMappedB = unmapComponent(c.blue, min.blue, max.blue);
        return new Color(unMappedR, unMappedG, unMappedB);
    }

    private static double mapComponent(double v, double min, double max) {
        return MathUtil.clamp((v - min) / (max - min), min, max);
    }

    private static double unmapComponent(double v, double min, double max) {
        return v * (max - min) + min;
    }

    public static ResultMappableColorProvider fromDefaultValueAndMinMax(Color defaultValue, Color min, Color max) {
        return new ResultMappableColorProvider(
                new DoubleProvider(new MultiKeyframeBasedDoubleInterpolator(mapComponent(defaultValue.red, min.red, max.red))),
                new DoubleProvider(new MultiKeyframeBasedDoubleInterpolator(mapComponent(defaultValue.green, min.green, max.green))),
                new DoubleProvider(new MultiKeyframeBasedDoubleInterpolator(mapComponent(defaultValue.blue, min.blue, max.blue))),
                min,
                max);
    }

    @Override
    public Class<? extends DesSerFactory<? extends KeyframeableEffect>> generateSerializableContent() {
        return ResultMappableColorProviderFactory.class;
    }

    DoubleProvider getRedProvider() {
        return redProvider;
    }

    DoubleProvider getGreenProvider() {
        return greenProvider;
    }

    DoubleProvider getBlueProvider() {
        return blueProvider;
    }
}
