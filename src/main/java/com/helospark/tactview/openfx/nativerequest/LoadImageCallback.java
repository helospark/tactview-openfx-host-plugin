package com.helospark.tactview.openfx.nativerequest;

import com.sun.jna.Callback;

public interface LoadImageCallback extends Callback {
    public void loadImage(LoadImageRequest loadImageRequest);
}
