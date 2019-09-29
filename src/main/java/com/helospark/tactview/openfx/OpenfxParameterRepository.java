package com.helospark.tactview.openfx;

import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;

import javax.annotation.PostConstruct;

import com.helospark.lightdi.annotation.Component;
import com.helospark.tactview.core.timeline.effect.interpolation.KeyframeableEffect;
import com.helospark.tactview.core.timeline.message.ClipAddedMessage;
import com.helospark.tactview.core.timeline.message.EffectAddedMessage;
import com.helospark.tactview.core.util.messaging.MessagingService;

@Component
public class OpenfxParameterRepository {
    private ConcurrentHashMap<Integer, KeyframeableEffect> parameterMap = new ConcurrentHashMap<Integer, KeyframeableEffect>();
    private MessagingService messagingService;

    public OpenfxParameterRepository(MessagingService messagingService) {
        this.messagingService = messagingService;
    }

    @PostConstruct
    public void init() {
        messagingService.register(EffectAddedMessage.class, e -> {
            if (e.getEffect() instanceof OpenFXFilterEffect) {
                parameterMap.putAll(((OpenFXFilterEffect) e.getEffect()).getProviders());
            }
        });
        messagingService.register(ClipAddedMessage.class, e -> {
            if (e.getClip() instanceof OpenFXGeneratorProceduralClip) {
                parameterMap.putAll(((OpenFXGeneratorProceduralClip) e.getClip()).getProviders());
            }
        });
        //        messagingService.register(ClipRemovedMessage.class, e -> {
        //            if (e.getClip() instanceof OpenFXGeneratorProceduralClip) {
        //                parameterMap.removeAll(((OpenFXGeneratorProceduralClip) e.getClip()).getProviders());
        //            }
        //        });
        //        messagingService.register(EffectRemovedMessage.class, e -> {
        //            if (e.() instanceof OpenFXEffect) {
        //                parameterMap.removeAll(((OpenFXEffect) e.getEffect()).getProviders());
        //            }
        //        });
    }

    public Optional<KeyframeableEffect> findKeyframeableEffect(int id) {
        return Optional.ofNullable(parameterMap.get(id));
    }

}
