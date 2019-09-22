package com.helospark.tactview.openfx.nativerequest;

import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.List;

import com.sun.jna.Structure;

public class RenderImageRequest extends Structure implements Structure.ByReference {
    public int width;
    public int height;
    public int pluginIndex;
    public double time;
    public ByteBuffer returnValue;
    public ByteBuffer inputImage;

    @Override
    protected List<String> getFieldOrder() {
        return Arrays.asList("width", "height", "pluginIndex", "time", "returnValue", "inputImage");
    }
}
