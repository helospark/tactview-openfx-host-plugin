package com.helospark.tactview.openfx;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import com.fasterxml.jackson.databind.JsonNode;
import com.helospark.lightdi.annotation.Component;
import com.helospark.tactview.core.repository.ProjectRepository;
import com.helospark.tactview.core.save.LoadMetadata;
import com.helospark.tactview.core.timeline.effect.interpolation.KeyframeableEffect;
import com.helospark.tactview.core.timeline.effect.interpolation.ValueProviderDescriptor;
import com.helospark.tactview.core.util.ReflectionUtil;
import com.helospark.tactview.openfx.nativerequest.ClipInformation;
import com.helospark.tactview.openfx.nativerequest.ClipList;
import com.helospark.tactview.openfx.nativerequest.CreateInstanceRequest;
import com.helospark.tactview.openfx.nativerequest.Parameter;
import com.helospark.tactview.openfx.nativerequest.ParameterList;
import com.helospark.tactview.openfx.nativerequest.ParameterMap;

class OpenFxPluginInitializerResult {
    Map<Integer, KeyframeableEffect> providers = new HashMap<>();
    List<ValueProviderDescriptor> descriptors = new ArrayList<>();
    Map<String, KeyframeableEffect> nameToParameter;
    int createdInstanceIndex;
    private List<OpenfxClip> clips;

    public OpenFxPluginInitializerResult(Map<Integer, KeyframeableEffect> providers, List<ValueProviderDescriptor> descriptors, int createdInstanceIndex, Map<String, KeyframeableEffect> nameToParameter, List<OpenfxClip> clips) {
        this.providers = providers;
        this.descriptors = descriptors;
        this.createdInstanceIndex = createdInstanceIndex;
        this.nameToParameter = nameToParameter;
        this.clips = clips;
    }

}

@Component
public class OpenFxPluginInitializer {
    private ProjectRepository projectRepository;
    private OpenFxToTactviewParameterMapper parameterMapper;

    public OpenFxPluginInitializer(ProjectRepository projectRepository, OpenFxToTactviewParameterMapper parameterMapper) {
        this.projectRepository = projectRepository;
        this.parameterMapper = parameterMapper;
    }

    public OpenFxPluginInitializerResult initialize(int loadedPluginIndex, String pluginId, Map<String, KeyframeableEffect> previousValues) {
        CreateInstanceRequest createInstanceRequest = createRequest(loadedPluginIndex, pluginId);
        int createdInstanceIndex = OpenfxLibrary.INSTANCE.createInstance(createInstanceRequest);

        Map<String, OpenfxParameter> parameters = mapParameters(createInstanceRequest);
        List<OpenfxClip> clips = mapClips(createInstanceRequest);

        Map<String, KeyframeableEffect> parameterNameToEffect = parameterMapper.createKeyframeableEffects(parameters, clips, previousValues);

        var parameterMapperResult = parameterMapper.createDescriptors(parameterNameToEffect, parameters);

        return new OpenFxPluginInitializerResult(parameterMapperResult.getProviders(), parameterMapperResult.getDescriptors(), createdInstanceIndex, parameterMapperResult.getNameToEffect(), clips);
    }

    private List<OpenfxClip> mapClips(CreateInstanceRequest createInstanceRequest) {
        List<OpenfxClip> resultClips = new ArrayList<>();
        if (createInstanceRequest.clips.numberOfEntries > 0) {
            ClipInformation[] clips = (ClipInformation[]) createInstanceRequest.clips.clip.toArray(createInstanceRequest.clips.numberOfEntries);
            for (var element : clips) {
                resultClips.add(new OpenfxClip(element.name, element.isMask > 0));
            }
        }
        return resultClips;
    }

    public OpenFxPluginInitializerResult initializePluginAfterLoad(int loadedPluginIndex, String pluginId, JsonNode parametersNode, LoadMetadata loadMetadata) {
        CreateInstanceRequest createInstanceRequest = createRequest(loadedPluginIndex, pluginId);
        int createdInstanceIndex = OpenfxLibrary.INSTANCE.createInstance(createInstanceRequest);

        Map<String, OpenfxParameter> parameters = mapParameters(createInstanceRequest);
        List<OpenfxClip> clips = mapClips(createInstanceRequest);

        Map<String, KeyframeableEffect> parameterNameToEffect = parameterMapper.createKeyframeableEffects(parameters, clips, Collections.emptyMap());

        Map<String, KeyframeableEffect> mergedParameters = new LinkedHashMap<>();

        for (var parameter : parameterNameToEffect.entrySet()) {
            try {
                KeyframeableEffect deserializedParameter = parameter.getValue();
                try {
                    JsonNode parameterNode = parametersNode.get(parameter.getKey());
                    KeyframeableEffect previousParameterEffect = parameterNameToEffect.get(parameter.getKey());
                    deserializedParameter = ReflectionUtil.deserialize(parameterNode, KeyframeableEffect.class, previousParameterEffect, loadMetadata);
                } catch (Exception e) {
                    e.printStackTrace();
                }
                mergedParameters.put(parameter.getKey(), deserializedParameter);
            } catch (Exception e) {
                System.out.println("Unable to load parameter for openfx effect");
                e.printStackTrace();
            }
        }

        var parameterMapperResult = parameterMapper.createDescriptors(mergedParameters, parameters);

        return new OpenFxPluginInitializerResult(parameterMapperResult.getProviders(), parameterMapperResult.getDescriptors(), createdInstanceIndex, parameterMapperResult.getNameToEffect(), clips);
    }

    private CreateInstanceRequest createRequest(int loadedPluginIndex, String pluginId) {
        CreateInstanceRequest createInstanceRequest = new CreateInstanceRequest();
        createInstanceRequest.pluginIndex = loadedPluginIndex;
        createInstanceRequest.width = projectRepository.getWidth();
        createInstanceRequest.height = projectRepository.getHeight();
        createInstanceRequest.effectId = pluginId;
        createInstanceRequest.list = new ParameterList();
        createInstanceRequest.clips = new ClipList();
        return createInstanceRequest;
    }

    private Map<String, OpenfxParameter> mapParameters(CreateInstanceRequest createInstanceRequest) {
        Map<String, OpenfxParameter> parameters = new LinkedHashMap<>();
        Parameter[] parameter = (Parameter[]) createInstanceRequest.list.parameter.toArray(createInstanceRequest.list.numberOfParameters);
        for (int i = 0; i < createInstanceRequest.list.numberOfParameters; ++i) {
            ParameterMap[] paramMap = (ParameterMap[]) parameter[i].parameterMap.toArray(parameter[i].numberOfEntries);

            Map<String, List<String>> metadata = new HashMap<>();
            for (int j = 0; j < parameter[i].numberOfEntries; ++j) {
                String[] elements = paramMap[j].value.getStringArray(0, paramMap[j].numberOfValues);
                metadata.put(paramMap[j].key, Arrays.asList(elements));
            }

            parameters.put(parameter[i].name, OpenfxParameter.builder().withMetadata(metadata).withUniqueParameterId(parameter[i].uniqueParameterId).withName(parameter[i].name).withType(parameter[i].type).build());
        }
        return parameters;
    }

}
