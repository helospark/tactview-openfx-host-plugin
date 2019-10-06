package com.helospark.tactview.openfx.nativerequest;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import com.helospark.lightdi.annotation.Component;
import com.helospark.tactview.core.timeline.GetFrameRequest;
import com.helospark.tactview.core.timeline.StatelessEffect;
import com.helospark.tactview.core.timeline.TimelineClip;
import com.helospark.tactview.core.timeline.TimelineManagerAccessorInterface;
import com.helospark.tactview.core.timeline.TimelinePosition;
import com.helospark.tactview.core.timeline.VisualTimelineClip;
import com.helospark.tactview.core.timeline.image.ReadOnlyClipImage;

@Component
public class LoadImageImplementation implements LoadImageCallback {
    private TimelineManagerAccessorInterface timelineManagerAccessor;

    List<ReadOnlyClipImage> loadedImages = new ArrayList<>();

    public LoadImageImplementation(TimelineManagerAccessorInterface timelineManagerAccessor) {
        this.timelineManagerAccessor = timelineManagerAccessor;
    }

    @Override
    public void loadImage(LoadImageRequest loadImageRequest) {
        System.out.println("Loading image at time " + loadImageRequest.time);

        Optional<StatelessEffect> effect = timelineManagerAccessor.findEffectById(loadImageRequest.effectId);

        if (effect.isPresent()) {
            TimelineClip effectClip = timelineManagerAccessor.findClipForEffect(loadImageRequest.effectId).get();

            if (loadImageRequest.clipName.equals("Source")) {
                GetFrameRequest getFrameRequest = GetFrameRequest.builder()
                        .withApplyEffects(true)
                        .withApplyEffectsLessThanEffectChannel(effectClip.getEffectChannelIndex(loadImageRequest.effectId))
                        .withExpectedWidth(loadImageRequest.width)
                        .withExpectedHeight(loadImageRequest.height)
                        .withLowResolutionPreview(false)
                        .withPosition(TimelinePosition.ofSeconds(loadImageRequest.time))
                        .withScale(loadImageRequest.scale)
                        .withUseApproximatePosition(false)
                        .build();

                ReadOnlyClipImage result = ((VisualTimelineClip) effectClip).getFrame(getFrameRequest);

                loadedImages.add(result);

                loadImageRequest.data = result.getBuffer();
                loadImageRequest.width = result.getWidth();
                loadImageRequest.height = result.getHeight();
            } else {
                // ((OpenFXFilterEffect)effectClip).get
            }

        } else {
            TimelineClip clip = timelineManagerAccessor.findClipById(loadImageRequest.effectId).get();
            // ((OpenFXProceduralClip)clip).getFrame();
        }

    }

}
