package com.helospark.tactview.openfx;

import com.helospark.tactview.openfx.PluginHandler.LoadImageRequest;
import com.sun.jna.Callback;

public interface LoadImageCallback extends Callback {
    public void loadImage(LoadImageRequest loadImageRequest);
}
