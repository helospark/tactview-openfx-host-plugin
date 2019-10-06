package com.helospark.tactview.openfx.nativerequest;

import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.List;

import com.sun.jna.Structure;

public class LoadImageRequest extends Structure implements Structure.ByReference {
    public double time;
    public double scale;
    public int width;
    public int height;
    public String clipName;
    public String effectId;

    public ByteBuffer data;

    @Override
    protected List<String> getFieldOrder() {
        return Arrays.asList("time", "scale", "width", "height", "clipName", "effectId", "data");
    }
}