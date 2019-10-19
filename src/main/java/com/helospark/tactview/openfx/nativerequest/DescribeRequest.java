package com.helospark.tactview.openfx.nativerequest;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;

public class DescribeRequest extends Structure implements Structure.ByReference {
    public int pluginIndex;

    public String pluginId;
    public String name;
    public String description;
    public int supportedContextSize;
    public Pointer supportedContexts;

    @Override
    protected List<String> getFieldOrder() {
        return Arrays.asList("pluginIndex", "pluginId", "name", "description", "supportedContextSize", "supportedContexts");
    }
}