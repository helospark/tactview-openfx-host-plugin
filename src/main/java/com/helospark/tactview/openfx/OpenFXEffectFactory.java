package com.helospark.tactview.openfx;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.helospark.lightdi.annotation.Autowired;
import com.helospark.lightdi.annotation.Bean;
import com.helospark.lightdi.annotation.Configuration;
import com.helospark.tactview.core.repository.ProjectRepository;
import com.helospark.tactview.core.timeline.TimelineClipType;
import com.helospark.tactview.core.timeline.TimelineInterval;
import com.helospark.tactview.core.timeline.TimelineLength;
import com.helospark.tactview.core.timeline.effect.StandardEffectFactory;
import com.helospark.tactview.core.timeline.effect.TimelineEffectType;
import com.helospark.tactview.openfx.nativerequest.DescribeInContextRequest;
import com.helospark.tactview.openfx.nativerequest.DescribeRequest;
import com.helospark.tactview.openfx.nativerequest.InitializeHostRequest;
import com.helospark.tactview.openfx.nativerequest.LoadImageImplementation;
import com.helospark.tactview.openfx.nativerequest.LoadLibraryRequest;
import com.helospark.tactview.openfx.nativerequest.LoadPluginRequest;
import com.helospark.tactview.openfx.nativerequest.Parameter;
import com.helospark.tactview.openfx.nativerequest.ParameterList;
import com.helospark.tactview.openfx.nativerequest.ParameterMap;

@Configuration
public class OpenFXEffectFactory {
    @Autowired
    private ParameterResolverImplementation parameterResolverImplementation;

    @Bean
    public StandardEffectFactory openfxEffect(ProjectRepository projectRepository) {
        InitializeHostRequest initializeHostRequest = new InitializeHostRequest();
        initializeHostRequest.loadImageCallback = new LoadImageImplementation(831, 530, null); // DUMMY
        initializeHostRequest.parameterValueProviderCallback = parameterResolverImplementation;
        OpenfxLibrary.INSTANCE.initializeHost(initializeHostRequest);

        LoadLibraryRequest loadLibraryRequest = new LoadLibraryRequest();
        loadLibraryRequest.file = "/home/black/tmp/openfx-misc/Misc/Linux-64-debug/Misc.ofx";
        int libraryIndex = OpenfxLibrary.INSTANCE.loadLibrary(loadLibraryRequest);

        LoadPluginRequest loadPluginRequest = new LoadPluginRequest();
        loadPluginRequest.width = projectRepository.getWidth();
        loadPluginRequest.height = projectRepository.getHeight();
        loadPluginRequest.pluginIndex = 75;
        loadPluginRequest.libraryDescriptor = libraryIndex;;

        int pluginIndex = OpenfxLibrary.INSTANCE.loadPlugin(loadPluginRequest);

        DescribeRequest describeRequest = new DescribeRequest();

        OpenfxLibrary.INSTANCE.describe(describeRequest);

        System.out.println("name=" + describeRequest.name + "\n description=" + describeRequest.description);
        String[] supportedContexts = describeRequest.supportedContexts.getStringArray(0, describeRequest.supportedContextSize);
        System.out.println("Supported Contexts:");
        for (String context : supportedContexts) {
            System.out.println(" - " + context);
        }

        DescribeInContextRequest describeInContextRequest = new DescribeInContextRequest();
        describeInContextRequest.list = new ParameterList();

        OpenfxLibrary.INSTANCE.describeInContext(describeInContextRequest);

        List<OpenfxParameter> openfxParameters = new ArrayList<>();
        Parameter[] parameter = (Parameter[]) describeInContextRequest.list.parameter.toArray(describeInContextRequest.list.numberOfParameters);
        for (int i = 0; i < describeInContextRequest.list.numberOfParameters; ++i) {
            ParameterMap[] paramMap = (ParameterMap[]) parameter[i].parameterMap.toArray(parameter[i].numberOfEntries);

            Map<String, List<String>> metadata = new HashMap<>();
            for (int j = 0; j < parameter[i].numberOfEntries; ++j) {
                String[] elements = paramMap[j].value.getStringArray(0, paramMap[j].numberOfValues);
                metadata.put(paramMap[j].key, Arrays.asList(elements));
            }

            openfxParameters.add(OpenfxParameter.builder()
                    .withMetadata(metadata)
                    .withUniqueParameterId(parameter[i].uniqueParameterId)
                    .withName(parameter[i].name)
                    .withType(parameter[i].type)
                    .build());
        }

        return StandardEffectFactory.builder()
                .withFactory(request -> new OpenFXEffect(new TimelineInterval(request.getPosition(), TimelineLength.ofMillis(5000)), pluginIndex, openfxParameters, projectRepository))
                .withRestoreFactory((node, loadMetadata) -> new OpenFXEffect(node, loadMetadata))
                .withName("OpenFX")
                .withSupportedEffectId("OpenFX")
                .withSupportedClipTypes(List.of(TimelineClipType.VIDEO, TimelineClipType.IMAGE))
                .withEffectType(TimelineEffectType.VIDEO_EFFECT)
                .build();
    }

}
