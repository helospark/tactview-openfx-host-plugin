package com.helospark.tactview.openfx;

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

    @Override
    protected List<String> getFieldOrder() {
        return Arrays.asList("width", "height", "pluginIndex", "time", "returnValue");
    }
}
