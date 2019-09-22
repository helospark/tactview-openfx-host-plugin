package com.helospark.tactview.openfx;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Structure;

public class LoadPluginRequest extends Structure implements Structure.ByReference {
    public int libraryDescriptor;
    public int pluginIndex;
    public int width;
    public int height;

    @Override
    protected List<String> getFieldOrder() {
        return Arrays.asList("libraryDescriptor", "pluginIndex", "width", "height");
    }
}
