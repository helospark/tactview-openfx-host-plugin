package com.helospark.tactview.openfx;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import javax.annotation.PostConstruct;

import com.helospark.lightdi.annotation.Autowired;
import com.helospark.lightdi.annotation.Bean;
import com.helospark.lightdi.annotation.Configuration;
import com.helospark.tactview.core.decoder.ImageMetadata;
import com.helospark.tactview.core.repository.ProjectRepository;
import com.helospark.tactview.core.timeline.TimelineClipType;
import com.helospark.tactview.core.timeline.TimelineInterval;
import com.helospark.tactview.core.timeline.TimelineLength;
import com.helospark.tactview.core.timeline.effect.StandardEffectFactory;
import com.helospark.tactview.core.timeline.effect.TimelineEffectType;
import com.helospark.tactview.core.timeline.proceduralclip.ProceduralClipFactoryChainItem;
import com.helospark.tactview.core.timeline.proceduralclip.StandardProceduralClipFactoryChainItem;
import com.helospark.tactview.openfx.nativerequest.DescribeInContextRequest;
import com.helospark.tactview.openfx.nativerequest.DescribeRequest;
import com.helospark.tactview.openfx.nativerequest.InitializeHostRequest;
import com.helospark.tactview.openfx.nativerequest.LoadImageImplementation;
import com.helospark.tactview.openfx.nativerequest.LoadLibraryRequest;
import com.helospark.tactview.openfx.nativerequest.LoadPluginRequest;

@Configuration
public class OpenFXEffectFactory {
    private static final String GENERATOR_CONTEXT = "OfxImageEffectContextGenerator";
    private static final String FILTER_CONTEXT = "OfxImageEffectContextFilter";
    private static final String TRANSITION_CONTEXT = "OfxImageEffectContextTransition";
    @Autowired
    private ParameterResolverImplementation parameterResolverImplementation;
    @Autowired
    private ProjectRepository projectRepository;
    @Autowired
    private OpenFxPluginInitializer openFxPluginInitializer;

    private List<StandardEffectFactory> effectFactories;
    private List<ProceduralClipFactoryChainItem> proceduralClipFactories;
    private Map<String, Integer> pluginNameToLoadedPluginId = new HashMap<>();

    @PostConstruct
    public List<StandardEffectFactory> openfxEffect() {
        InitializeHostRequest initializeHostRequest = new InitializeHostRequest();
        initializeHostRequest.loadImageCallback = new LoadImageImplementation(831, 530, null); // DUMMY
        initializeHostRequest.parameterValueProviderCallback = parameterResolverImplementation;
        OpenfxLibrary.INSTANCE.initializeHost(initializeHostRequest);

        LoadLibraryRequest loadLibraryRequest = new LoadLibraryRequest();
        loadLibraryRequest.file = "/home/black/tmp/openfx-misc/Misc/Linux-64-debug/Misc.ofx";
        int libraryIndex = OpenfxLibrary.INSTANCE.loadLibrary(loadLibraryRequest);

        effectFactories = new ArrayList<>();
        proceduralClipFactories = new ArrayList<>();

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

            String pluginName = describeRequest.name;
            pluginNameToLoadedPluginId.put(pluginName, loadedPluginIndex);

            List<String> supportedContexts = Arrays.asList(describeRequest.supportedContexts.getStringArray(0, describeRequest.supportedContextSize));

            if (supportedContexts.contains(FILTER_CONTEXT)) {
                createEffectFactory(loadedPluginIndex, pluginName).ifPresent(effectFactories::add);
            } else if (supportedContexts.contains(GENERATOR_CONTEXT)) {
                createProcuduralClipFactory(loadedPluginIndex, pluginName).ifPresent(proceduralClipFactories::add);
            } else if (supportedContexts.contains(TRANSITION_CONTEXT)) {
                createTransitionEffectFactory(loadedPluginIndex, pluginName).ifPresent(effectFactories::add);
            }
        }
        return effectFactories;

    }

    private Optional<StandardEffectFactory> createEffectFactory(int loadedPluginIndex, String pluginName) {
        Optional<StandardEffectFactory> effectFactory = Optional.empty();
        DescribeInContextRequest describeInContextRequest = new DescribeInContextRequest();
        describeInContextRequest.pluginIndex = loadedPluginIndex;
        describeInContextRequest.context = FILTER_CONTEXT;
        int returnStatus = OpenfxLibrary.INSTANCE.describeInContext(describeInContextRequest);

        if (returnStatus >= 0) {
            effectFactory = Optional.of(StandardEffectFactory.builder()
                    .withFactory(request -> new OpenFXFilterEffect(new TimelineInterval(request.getPosition(), TimelineLength.ofMillis(5000)), loadedPluginIndex, openFxPluginInitializer,
                            pluginName))
                    .withRestoreFactory((node, loadMetadata) -> new OpenFXFilterEffect(node, loadMetadata, openFxPluginInitializer, pluginNameToLoadedPluginId))
                    .withName(pluginName + "-openfx")
                    .withSupportedEffectId("openfx-filter-" + pluginName)
                    .withSupportedClipTypes(List.of(TimelineClipType.VIDEO, TimelineClipType.IMAGE))
                    .withEffectType(TimelineEffectType.VIDEO_EFFECT)
                    .build());
        }
        return effectFactory;
    }

    private Optional<ProceduralClipFactoryChainItem> createProcuduralClipFactory(int loadedPluginIndex, String pluginName) {
        DescribeInContextRequest describeInContextRequest = new DescribeInContextRequest();
        describeInContextRequest.pluginIndex = loadedPluginIndex;
        describeInContextRequest.context = GENERATOR_CONTEXT;
        int returnStatus = OpenfxLibrary.INSTANCE.describeInContext(describeInContextRequest);

        if (returnStatus >= 0) {
            TimelineLength defaultLength = TimelineLength.ofMillis(30000);
            ImageMetadata metadata = ImageMetadata.builder()
                    .withWidth(1920)
                    .withHeight(1080)
                    .withLength(defaultLength)
                    .build();

            ProceduralClipFactoryChainItem proceduralClipFactory = new StandardProceduralClipFactoryChainItem("openfx-generator-" + pluginName, pluginName + "-openfx",
                    request -> {
                        return new OpenFXGeneratorProceduralClip(metadata, new TimelineInterval(request.getPosition(), defaultLength), loadedPluginIndex, openFxPluginInitializer,
                                pluginName);
                    },
                    (node, loadMetadata) -> {
                        return new OpenFXGeneratorProceduralClip(metadata, node, loadMetadata, openFxPluginInitializer, pluginNameToLoadedPluginId);
                    });
            return Optional.of(proceduralClipFactory);
        } else {
            return Optional.empty();
        }
    }

    private Optional<StandardEffectFactory> createTransitionEffectFactory(int loadedPluginIndex, String pluginName) {
        Optional<StandardEffectFactory> effectFactory = Optional.empty();
        DescribeInContextRequest describeInContextRequest = new DescribeInContextRequest();
        describeInContextRequest.pluginIndex = loadedPluginIndex;
        describeInContextRequest.context = TRANSITION_CONTEXT;
        int returnStatus = OpenfxLibrary.INSTANCE.describeInContext(describeInContextRequest);

        if (returnStatus >= 0) {
            effectFactory = Optional.of(StandardEffectFactory.builder()
                    .withFactory(request -> new OpenFXTransitionEffect(new TimelineInterval(request.getPosition(), TimelineLength.ofMillis(2000)), loadedPluginIndex, openFxPluginInitializer,
                            pluginName))
                    .withRestoreFactory((node, loadMetadata) -> new OpenFXFilterEffect(node, loadMetadata, openFxPluginInitializer, pluginNameToLoadedPluginId))
                    .withName(pluginName + "-openfx")
                    .withSupportedEffectId("openfx-transition-" + pluginName)
                    .withSupportedClipTypes(List.of(TimelineClipType.VIDEO, TimelineClipType.IMAGE))
                    .withEffectType(TimelineEffectType.VIDEO_TRANSITION)
                    .build());
        }
        return effectFactory;
    }

    @Bean
    public List<StandardEffectFactory> getEffectFactories() {
        return effectFactories;
    }

    @Bean
    public List<ProceduralClipFactoryChainItem> getProceduralClipFactories() {
        return proceduralClipFactories;
    }

}
