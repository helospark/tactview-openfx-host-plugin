package com.helospark.tactview.openfx.nativerequest;

import java.nio.ByteBuffer;

public class LoadImageImplementation implements LoadImageCallback {
    int width;
    int height;
    ByteBuffer data;

    public LoadImageImplementation(int width, int height, ByteBuffer data) {
        this.width = width;
        this.height = height;
        this.data = data;
    }

    @Override
    public void loadImage(LoadImageRequest loadImageRequest) {
        System.out.println("Loading image at time " + loadImageRequest.time);

        loadImageRequest.width = width;
        loadImageRequest.height = height;
        loadImageRequest.data = data;
    }

}
