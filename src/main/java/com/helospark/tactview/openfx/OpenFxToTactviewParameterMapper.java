package com.helospark.tactview.openfx;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import com.helospark.lightdi.annotation.Component;
import com.helospark.tactview.core.timeline.effect.interpolation.KeyframeableEffect;
import com.helospark.tactview.core.timeline.effect.interpolation.ValueProviderDescriptor;
import com.helospark.tactview.core.timeline.effect.interpolation.interpolator.StepStringInterpolator;
import com.helospark.tactview.core.timeline.effect.interpolation.interpolator.bezier.BezierDoubleInterpolator;
import com.helospark.tactview.core.timeline.effect.interpolation.pojo.Color;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.BooleanProvider;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.ColorProvider;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.DoubleProvider;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.IntegerProvider;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.ValueListElement;
import com.helospark.tactview.core.timeline.effect.interpolation.provider.ValueListProvider;

class ParameterMapperResult {
    private Map<Integer, KeyframeableEffect> providers;
    private List<ValueProviderDescriptor> descriptors;
    private Map<String, KeyframeableEffect> nameToEffect;

    public ParameterMapperResult(Map<Integer, KeyframeableEffect> providers, List<ValueProviderDescriptor> descriptors, Map<String, KeyframeableEffect> nameToEffect) {
        this.providers = providers;
        this.descriptors = descriptors;
        this.nameToEffect = nameToEffect;
    }

    public Map<Integer, KeyframeableEffect> getProviders() {
        return providers;
    }

    public List<ValueProviderDescriptor> getDescriptors() {
        return descriptors;
    }

    public Map<String, KeyframeableEffect> getNameToEffect() {
        return nameToEffect;
    }

}

@Component
public class OpenFxToTactviewParameterMapper {

    public Map<String, KeyframeableEffect> createKeyframeableEffects(Map<String, OpenfxParameter> parameters, Map<String, KeyframeableEffect> previousValues) {
        Map<String, KeyframeableEffect> nameToEffect = new LinkedHashMap<>();
        for (var parameterEntry : parameters.entrySet()) {
            var parameter = parameterEntry.getValue();
            String type = parameter.getType();
            String name = parameter.getName();

            if (previousValues.containsKey(name)) {
                nameToEffect.put(name, previousValues.get(name));
            } else {
                if (type.equals("OfxParamTypeBoolean")) {
                    double defaultValue = getSingleMetadata(parameter, "OfxParamPropDefault")
                            .map(a -> Boolean.valueOf(a))
                            .map(a -> a ? 1.0 : 0.0)
                            .orElse(0.0);
                    BooleanProvider booleanProvider = new BooleanProvider(new BezierDoubleInterpolator(defaultValue));
                    nameToEffect.put(parameter.getName(), booleanProvider);
                } else if (type.equals("OfxParamTypeDouble")) {
                    double defaultValue = getSingleMetadata(parameter, "OfxParamPropDefault")
                            .map(a -> Double.valueOf(a))
                            .orElse(0.0);
                    DoubleProvider doubleProvider = new DoubleProvider(new BezierDoubleInterpolator(defaultValue));
                    nameToEffect.put(parameter.getName(), doubleProvider);
                } else if (type.equals("OfxParamTypeInteger")) {
                    double defaultValue = getSingleMetadata(parameter, "OfxParamPropDefault")
                            .map(a -> Integer.valueOf(a))
                            .orElse(0);
                    int min = getSingleMetadata(parameter, "OfxParamPropDisplayMin")
                            .map(a -> Integer.valueOf(a))
                            .orElse(0);
                    int max = getSingleMetadata(parameter, "OfxParamPropDisplayMax")
                            .map(a -> Integer.valueOf(a))
                            .orElse(10);
                    IntegerProvider intProvider = new IntegerProvider(min, max, new BezierDoubleInterpolator(defaultValue));
                    nameToEffect.put(parameter.getName(), intProvider);
                } else if (type.equals("OfxParamTypeChoice")) {
                    int defaultValue = getSingleMetadata(parameter, "OfxParamPropDefault")
                            .map(a -> Integer.valueOf(a))
                            .orElse(0);

                    List<ValueListElement> elements = mapValuesToListElements(parameter.getMetadata().getOrDefault("OfxParamPropChoiceOption", List.of()));

                    ValueListProvider<ValueListElement> provider = new ValueListProvider<>(elements, new StepStringInterpolator(String.valueOf(defaultValue)));
                    nameToEffect.put(parameter.getName(), provider);
                } else if (type.equals("OfxParamTypeRGB")) {
                    Color color = getColorFrom(parameter, "OfxParamPropDefault");
                    Color min = getColorFrom(parameter, "OfxParamPropMin");
                    Color max = getColorFrom(parameter, "OfxParamPropMax");

                    ColorProvider provider = ResultMappableColorProvider.fromDefaultValueAndMinMax(color, min, max);
                    nameToEffect.put(parameter.getName(), provider);
                }
            }
        }

        return nameToEffect;
    }

    public static class OpenfxValueListElement extends ValueListElement {
        public int openfxId;

        public OpenfxValueListElement(String text, int openfxId) {
            super(String.valueOf(openfxId), text);
            this.openfxId = openfxId;
        }

    }

    private Color getColorFrom(OpenfxParameter parameter, String string) {
        parameter.getMetadata().get(string);

        double r = getSingleMetadata(parameter, string, 0)
                .map(a -> Double.valueOf(a))
                .orElse(0.0);
        double g = getSingleMetadata(parameter, string, 0)
                .map(a -> Double.valueOf(a))
                .orElse(0.0);
        double b = getSingleMetadata(parameter, string, 0)
                .map(a -> Double.valueOf(a))
                .orElse(0.0);

        return new Color(r, g, b);
    }

    private List<ValueListElement> mapValuesToListElements(List<String> list) {
        List<ValueListElement> result = new ArrayList<>();
        for (int i = 0; i < list.size(); ++i) {
            result.add(new OpenfxValueListElement(list.get(i), i));
        }
        return result;
    }

    private Optional<String> getSingleMetadata(OpenfxParameter parameter, String string) {
        return getSingleMetadata(parameter, string, 0);
    }

    private Optional<String> getSingleMetadata(OpenfxParameter parameter, String string, int i) {
        return Optional.ofNullable(parameter.getMetadata().get(string))
                .filter(a -> a.size() > 0)
                .map(a -> a.get(i));
    }

    public ParameterMapperResult createDescriptors(Map<String, KeyframeableEffect> mergedParameters, Map<String, OpenfxParameter> parameters) {
        Map<Integer, KeyframeableEffect> providers = new LinkedHashMap<>();
        List<ValueProviderDescriptor> descriptors = new ArrayList<>();
        for (var entry : mergedParameters.entrySet()) {
            OpenfxParameter parameter = parameters.get(entry.getKey());
            int uniqueParameterId = parameter.getUniqueParameterId();
            providers.put(uniqueParameterId, entry.getValue());
            descriptors.add(ValueProviderDescriptor.builder()
                    .withKeyframeableEffect(entry.getValue())
                    .withName(parameter.getName())
                    .build());
        }
        return new ParameterMapperResult(providers, descriptors, mergedParameters);
    }

}
