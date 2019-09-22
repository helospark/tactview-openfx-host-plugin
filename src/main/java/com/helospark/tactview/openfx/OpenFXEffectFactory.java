package com.helospark.tactview.openfx;

import java.util.List;

import com.helospark.lightdi.annotation.Bean;
import com.helospark.lightdi.annotation.Configuration;
import com.helospark.tactview.core.repository.ProjectRepository;
import com.helospark.tactview.core.timeline.TimelineClipType;
import com.helospark.tactview.core.timeline.TimelineInterval;
import com.helospark.tactview.core.timeline.TimelineLength;
import com.helospark.tactview.core.timeline.effect.StandardEffectFactory;
import com.helospark.tactview.core.timeline.effect.TimelineEffectType;
import com.helospark.tactview.openfx.nativerequest.CreateInstanceRequest;
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

    @Bean
    public StandardEffectFactory openfxEffect(ProjectRepository projectRepository) {
        InitializeHostRequest initializeHostRequest = new InitializeHostRequest();
        initializeHostRequest.loadImageCallback = new LoadImageImplementation(831, 530, null); // DUMMY
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

        CreateInstanceRequest createInstanceRequest = new CreateInstanceRequest();
        createInstanceRequest.width = loadPluginRequest.width;
        createInstanceRequest.height = loadPluginRequest.height;
        OpenfxLibrary.INSTANCE.createInstance(createInstanceRequest);

        System.out.println("Parameters:");
        Parameter[] parameter = (Parameter[]) describeInContextRequest.list.parameter.toArray(describeInContextRequest.list.numberOfParameters);
        for (int i = 0; i < describeInContextRequest.list.numberOfParameters; ++i) {
            System.out.println(parameter[i].name + "   " + parameter[i].type);
            ParameterMap[] paramMap = (ParameterMap[]) parameter[i].parameterMap.toArray(parameter[i].numberOfEntries);
            for (int j = 0; j < parameter[i].numberOfEntries; ++j) {
                System.out.print("key=" + paramMap[j].key + " : ");
                String[] elements = paramMap[j].value.getStringArray(0, paramMap[j].numberOfValues);
                for (String element : elements) {
                    System.out.print(element + " ");
                }
                System.out.println();
            }
            System.out.println("\n----\n");
        }

        return StandardEffectFactory.builder()
                .withFactory(request -> new OpenFXEffect(new TimelineInterval(request.getPosition(), TimelineLength.ofMillis(5000)), pluginIndex))
                .withRestoreFactory((node, loadMetadata) -> new OpenFXEffect(node, loadMetadata))
                .withName("OpenFX")
                .withSupportedEffectId("OpenFX")
                .withSupportedClipTypes(List.of(TimelineClipType.VIDEO, TimelineClipType.IMAGE))
                .withEffectType(TimelineEffectType.VIDEO_EFFECT)
                .build();
    }

}
