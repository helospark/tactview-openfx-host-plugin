package com.helospark.tactview.openfx.nativerequest;

import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.List;

import com.sun.jna.Structure;

public class RenderImageClip extends Structure implements Structure.ByReference {
    public int width;
    public int height;
    public String name;
    public ByteBuffer data;

    @Override
    protected List<String> getFieldOrder() {
        return Arrays.asList("width", "height", "name", "data");
    }
}
