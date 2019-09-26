package com.helospark.tactview.openfx;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import com.fasterxml.jackson.databind.JsonNode;
import com.helospark.tactview.core.clone.CloneRequestMetadata;
import com.helospark.tactview.core.decoder.framecache.GlobalMemoryManagerAccessor;
import com.helospark.tactview.core.repository.ProjectRepository;
import com.helospark.tactview.core.save.LoadMetadata;
import com.helospark.tactview.core.timeline.StatelessEffect;
import com.helospark.tactview.core.timeline.StatelessVideoEffect;
import com.helospark.tactview.core.timeline.TimelineInterval;
import com.helospark.tactview.core.timeline.effect.StatelessEffectRequest;
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
import com.helospark.tactview.core.timeline.image.ClipImage;
import com.helospark.tactview.core.timeline.image.ReadOnlyClipImage;
import com.helospark.tactview.core.util.ReflectionUtil;
import com.helospark.tactview.openfx.nativerequest.CreateInstanceRequest;
import com.helospark.tactview.openfx.nativerequest.DescribeInContextRequest;
import com.helospark.tactview.openfx.nativerequest.Parameter;
import com.helospark.tactview.openfx.nativerequest.ParameterList;
import com.helospark.tactview.openfx.nativerequest.ParameterMap;
import com.helospark.tactview.openfx.nativerequest.RenderImageRequest;

public class OpenFXEffect extends StatelessVideoEffect {
    private int pluginIndex;
    private List<OpenfxParameter> parameters;
    private Map<Integer, KeyframeableEffect> providers = new HashMap<>();
    private List<ValueProviderDescriptor> descriptors = new ArrayList<>();

    public OpenFXEffect(TimelineInterval interval, int pluginIndex, ProjectRepository projectRepository) {
        super(interval);
        this.pluginIndex = pluginIndex;

        DescribeInContextRequest describeInContextRequest = new DescribeInContextRequest();
        describeInContextRequest.pluginIndex = pluginIndex;
        describeInContextRequest.list = new ParameterList();

        OpenfxLibrary.INSTANCE.describeInContext(describeInContextRequest);

        CreateInstanceRequest createInstanceRequest = new CreateInstanceRequest();
        createInstanceRequest.pluginIndex = pluginIndex;
        createInstanceRequest.width = projectRepository.getWidth();
        createInstanceRequest.height = projectRepository.getHeight();
        createInstanceRequest.effectId = getId();
        OpenfxLibrary.INSTANCE.createInstance(createInstanceRequest);

        parameters = new ArrayList<>();
        Parameter[] parameter = (Parameter[]) describeInContextRequest.list.parameter.toArray(describeInContextRequest.list.numberOfParameters);
        for (int i = 0; i < describeInContextRequest.list.numberOfParameters; ++i) {
            ParameterMap[] paramMap = (ParameterMap[]) parameter[i].parameterMap.toArray(parameter[i].numberOfEntries);

            Map<String, List<String>> metadata = new HashMap<>();
            for (int j = 0; j < parameter[i].numberOfEntries; ++j) {
                String[] elements = paramMap[j].value.getStringArray(0, paramMap[j].numberOfValues);
                metadata.put(paramMap[j].key, Arrays.asList(elements));
            }

            parameters.add(OpenfxParameter.builder()
                    .withMetadata(metadata)
                    .withUniqueParameterId(parameter[i].uniqueParameterId)
                    .withName(parameter[i].name)
                    .withType(parameter[i].type)
                    .build());
        }
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
            renderImageRequest.effectId = getId();

            OpenfxLibrary.INSTANCE.renderImage(renderImageRequest);

            ClipImage clipImage = new ClipImage(result, width, height);
            return clipImage;
        }
    }

    @Override
    public void initializeValueProvider() {
        for (var parameter : parameters) {
            String type = parameter.getType();
            if (type.equals("OfxParamTypeBoolean")) {
                double defaultValue = getSingleMetadata(parameter, "OfxParamPropDefault")
                        .map(a -> Boolean.valueOf(a))
                        .map(a -> a ? 1.0 : 0.0)
                        .orElse(0.0);
                BooleanProvider booleanProvider = new BooleanProvider(new BezierDoubleInterpolator(defaultValue));

                ValueProviderDescriptor heightDescriptor = ValueProviderDescriptor.builder()
                        .withKeyframeableEffect(booleanProvider)
                        .withName(parameter.getName())
                        .build();
                providers.put(parameter.getUniqueParameterId(), booleanProvider);
                descriptors.add(heightDescriptor);
            } else if (type.equals("OfxParamTypeDouble")) {
                double defaultValue = getSingleMetadata(parameter, "OfxParamPropDefault")
                        .map(a -> Double.valueOf(a))
                        .orElse(0.0);
                DoubleProvider doubleProvider = new DoubleProvider(new BezierDoubleInterpolator(defaultValue));

                ValueProviderDescriptor heightDescriptor = ValueProviderDescriptor.builder()
                        .withKeyframeableEffect(doubleProvider)
                        .withName(parameter.getName())
                        .build();
                providers.put(parameter.getUniqueParameterId(), doubleProvider);
                descriptors.add(heightDescriptor);
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

                ValueProviderDescriptor heightDescriptor = ValueProviderDescriptor.builder()
                        .withKeyframeableEffect(intProvider)
                        .withName(parameter.getName())
                        .build();
                providers.put(parameter.getUniqueParameterId(), intProvider);
                descriptors.add(heightDescriptor);
            } else if (type.equals("OfxParamTypeChoice")) {
                int defaultValue = getSingleMetadata(parameter, "OfxParamPropDefault")
                        .map(a -> Integer.valueOf(a))
                        .orElse(0);

                List<ValueListElement> elements = mapValuesToListElements(parameter.getMetadata().getOrDefault("OfxParamPropChoiceOption", List.of()));

                ValueListProvider<ValueListElement> provider = new ValueListProvider<>(elements, new StepStringInterpolator(String.valueOf(defaultValue)));

                ValueProviderDescriptor heightDescriptor = ValueProviderDescriptor.builder()
                        .withKeyframeableEffect(provider)
                        .withName(parameter.getName())
                        .build();
                providers.put(parameter.getUniqueParameterId(), provider);
                descriptors.add(heightDescriptor);
            } else if (type.equals("OfxParamTypeRGB")) {
                Color color = getColorFrom(parameter, "OfxParamPropDefault");
                Color min = getColorFrom(parameter, "OfxParamPropMin");
                Color max = getColorFrom(parameter, "OfxParamPropMax");

                ColorProvider provider = ResultMappableColorProvider.fromDefaultValueAndMinMax(color, min, max);

                ValueProviderDescriptor heightDescriptor = ValueProviderDescriptor.builder()
                        .withKeyframeableEffect(provider)
                        .withName(parameter.getName())
                        .build();
                providers.put(parameter.getUniqueParameterId(), provider);
                descriptors.add(heightDescriptor);
            }
        }
    }

    public static class OpenfxValueListElement extends ValueListElement {
        public int openfxId;

        public OpenfxValueListElement(String text, int openfxId) {
            super(text, text);
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

    @Override
    public List<ValueProviderDescriptor> getValueProviders() {
        return descriptors;
    }

    @Override
    public StatelessEffect cloneEffect(CloneRequestMetadata cloneRequestMetadata) {
        return new OpenFXEffect(this, cloneRequestMetadata);
    }

    public Map<Integer, KeyframeableEffect> getProviders() {
        return providers;
    }

}
