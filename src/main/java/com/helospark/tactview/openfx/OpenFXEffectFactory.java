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

@Configuration
public class OpenFXEffectFactory {
    @Autowired
    private ParameterResolverImplementation parameterResolverImplementation;
    //    @Autowired
    //    private LoadPluginService loadPluginService;

    @Bean
    public List<StandardEffectFactory> openfxEffect(ProjectRepository projectRepository, OpenFxPluginInitializer openFxPluginInitializer) {
        InitializeHostRequest initializeHostRequest = new InitializeHostRequest();
        initializeHostRequest.loadImageCallback = new LoadImageImplementation(831, 530, null); // DUMMY
        initializeHostRequest.parameterValueProviderCallback = parameterResolverImplementation;
        OpenfxLibrary.INSTANCE.initializeHost(initializeHostRequest);

        LoadLibraryRequest loadLibraryRequest = new LoadLibraryRequest();
        loadLibraryRequest.file = "/home/black/tmp/openfx-misc/Misc/Linux-64-debug/Misc.ofx";
        int libraryIndex = OpenfxLibrary.INSTANCE.loadLibrary(loadLibraryRequest);

        List<StandardEffectFactory> effectFactories = new ArrayList<>();

        Map<String, Integer> pluginNameToLoadedPluginId = new HashMap<>();

        for (int i = 0; i < loadLibraryRequest.numberOfPlugins; ++i) {
            LoadPluginRequest loadPluginRequest = new LoadPluginRequest();
            loadPluginRequest.width = projectRepository.getWidth();
            loadPluginRequest.height = projectRepository.getHeight();
            loadPluginRequest.pluginIndex = i;
            loadPluginRequest.libraryDescriptor = libraryIndex;
            int loadedPluginIndex = OpenfxLibrary.INSTANCE.loadPlugin(loadPluginRequest);

            DescribeRequest describeRequest = new DescribeRequest();
            describeRequest.pluginIndex = loadedPluginIndex;
            OpenfxLibrary.INSTANCE.describe(describeRequest);

            pluginNameToLoadedPluginId.put(describeRequest.name, loadedPluginIndex);

            List<String> supportedContexts = Arrays.asList(describeRequest.supportedContexts.getStringArray(0, describeRequest.supportedContextSize));

            if (supportedContexts.contains("OfxImageEffectContextFilter")) {
                DescribeInContextRequest describeInContextRequest = new DescribeInContextRequest();
                describeInContextRequest.pluginIndex = loadedPluginIndex;
                int returnStatus = OpenfxLibrary.INSTANCE.describeInContext(describeInContextRequest);

                if (returnStatus >= 0) {
                    StandardEffectFactory effectFactory = StandardEffectFactory.builder()
                            .withFactory(request -> new OpenFXEffect(new TimelineInterval(request.getPosition(), TimelineLength.ofMillis(5000)), loadedPluginIndex, openFxPluginInitializer,
                                    describeRequest.name))
                            .withRestoreFactory((node, loadMetadata) -> new OpenFXEffect(node, loadMetadata, openFxPluginInitializer, pluginNameToLoadedPluginId))
                            .withName(describeRequest.name + "-openfx")
                            .withSupportedEffectId("openfx-" + describeRequest.name)
                            .withSupportedClipTypes(List.of(TimelineClipType.VIDEO, TimelineClipType.IMAGE))
                            .withEffectType(TimelineEffectType.VIDEO_EFFECT)
                            .build();
                    effectFactories.add(effectFactory);
                }
            }
        }
        return effectFactories;

    }

}
