package com.helospark.tactview.openfx;

import java.io.File;
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
    @Autowired
    private OpenFXPluginPathProvider openFXPluginPathProvider;
    @Autowired
    private LoadImageImplementation loadImageImplementation;

    private List<StandardEffectFactory> effectFactories;
    private List<ProceduralClipFactoryChainItem> proceduralClipFactories;
    private Map<String, Integer> pluginNameToLoadedPluginId = new HashMap<>();

    private Map<String, PluginDescription> idToPluginDescription = new HashMap<>();

    @PostConstruct
    public List<StandardEffectFactory> openfxEffect() {
        effectFactories = new ArrayList<>();
        proceduralClipFactories = new ArrayList<>();

        InitializeHostRequest initializeHostRequest = new InitializeHostRequest();
        initializeHostRequest.loadImageCallback = loadImageImplementation;
        initializeHostRequest.parameterValueProviderCallback = parameterResolverImplementation;
        OpenfxLibrary.INSTANCE.initializeHost(initializeHostRequest);

        for (File bundle : openFXPluginPathProvider.getOpenFxBundles()) {
            LoadLibraryRequest loadLibraryRequest = new LoadLibraryRequest();
            loadLibraryRequest.file = bundle.getAbsolutePath();
            int libraryIndex = OpenfxLibrary.INSTANCE.loadLibrary(loadLibraryRequest);

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
                    createEffectFactory(loadedPluginIndex, pluginName, describeRequest, bundle).ifPresent(effectFactories::add);
                } else if (supportedContexts.contains(GENERATOR_CONTEXT)) {
                    createProcuduralClipFactory(loadedPluginIndex, pluginName, describeRequest, bundle).ifPresent(proceduralClipFactories::add);
                } else if (supportedContexts.contains(TRANSITION_CONTEXT)) {
                    createTransitionEffectFactory(loadedPluginIndex, pluginName, describeRequest, bundle).ifPresent(effectFactories::add);
                }
            }
        }
        return effectFactories;

    }

    private Optional<StandardEffectFactory> createEffectFactory(int loadedPluginIndex, String pluginName, DescribeRequest describeRequest, File bundle) {
        Optional<StandardEffectFactory> effectFactory = Optional.empty();
        DescribeInContextRequest describeInContextRequest = new DescribeInContextRequest();
        describeInContextRequest.pluginIndex = loadedPluginIndex;
        describeInContextRequest.context = FILTER_CONTEXT;
        int returnStatus = OpenfxLibrary.INSTANCE.describeInContext(describeInContextRequest);

        if (returnStatus >= 0) {
            String id = "openfx-filter-" + pluginName;
            effectFactory = Optional.of(StandardEffectFactory.builder()
                    .withFactory(request -> new OpenFXFilterEffect(new TimelineInterval(request.getPosition(), TimelineLength.ofMillis(5000)), loadedPluginIndex, openFxPluginInitializer,
                            pluginName))
                    .withRestoreFactory((node, loadMetadata) -> new OpenFXFilterEffect(node, loadMetadata, openFxPluginInitializer, pluginNameToLoadedPluginId))
                    .withName(pluginName + "-openfx")
                    .withSupportedEffectId(id)
                    .withSupportedClipTypes(List.of(TimelineClipType.VIDEO, TimelineClipType.IMAGE))
                    .withEffectType(TimelineEffectType.VIDEO_EFFECT)
                    .build());
            PluginDescription pluginDescription = PluginDescription.builder()
                    .withDescription(describeRequest.description)
                    .withLibraryPath(bundle)
                    .withPluginId(describeRequest.pluginId)
                    .build();
            idToPluginDescription.put(id, pluginDescription);
        }
        return effectFactory;
    }

    private Optional<ProceduralClipFactoryChainItem> createProcuduralClipFactory(int loadedPluginIndex, String pluginName, DescribeRequest describeRequest, File bundle) {
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

            String id = "openfx-generator-" + pluginName;
            ProceduralClipFactoryChainItem proceduralClipFactory = new StandardProceduralClipFactoryChainItem(id, pluginName + "-openfx",
                    request -> {
                        return new OpenFXGeneratorProceduralClip(metadata, new TimelineInterval(request.getPosition(), defaultLength), loadedPluginIndex, openFxPluginInitializer,
                                pluginName);
                    },
                    (node, loadMetadata) -> {
                        return new OpenFXGeneratorProceduralClip(metadata, node, loadMetadata, openFxPluginInitializer, pluginNameToLoadedPluginId);
                    });
            PluginDescription pluginDescription = PluginDescription.builder()
                    .withDescription(describeRequest.description)
                    .withLibraryPath(bundle)
                    .withPluginId(describeRequest.pluginId)
                    .build();
            idToPluginDescription.put(id, pluginDescription);
            return Optional.of(proceduralClipFactory);
        } else {
            return Optional.empty();
        }
    }

    private Optional<StandardEffectFactory> createTransitionEffectFactory(int loadedPluginIndex, String pluginName, DescribeRequest describeRequest, File bundle) {
        Optional<StandardEffectFactory> effectFactory = Optional.empty();
        DescribeInContextRequest describeInContextRequest = new DescribeInContextRequest();
        describeInContextRequest.pluginIndex = loadedPluginIndex;
        describeInContextRequest.context = TRANSITION_CONTEXT;
        int returnStatus = OpenfxLibrary.INSTANCE.describeInContext(describeInContextRequest);

        if (returnStatus >= 0) {
            String id = "openfx-transition-" + pluginName;
            effectFactory = Optional.of(StandardEffectFactory.builder()
                    .withFactory(request -> new OpenFXTransitionEffect(new TimelineInterval(request.getPosition(), TimelineLength.ofMillis(2000)), loadedPluginIndex, openFxPluginInitializer,
                            pluginName))
                    .withRestoreFactory((node, loadMetadata) -> new OpenFXFilterEffect(node, loadMetadata, openFxPluginInitializer, pluginNameToLoadedPluginId))
                    .withName(pluginName + "-openfx")
                    .withSupportedEffectId(id)
                    .withSupportedClipTypes(List.of(TimelineClipType.VIDEO, TimelineClipType.IMAGE))
                    .withEffectType(TimelineEffectType.VIDEO_TRANSITION)
                    .build());

            PluginDescription pluginDescription = PluginDescription.builder()
                    .withDescription(describeRequest.description)
                    .withLibraryPath(bundle)
                    .withPluginId(describeRequest.pluginId)
                    .build();
            idToPluginDescription.put(id, pluginDescription);
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

    public Map<String, PluginDescription> getIdToPluginDescription() {
        return idToPluginDescription;
    }

}
