package com.helospark.tactview.openfx.nativerequest;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Structure;

public class DescribeInContextRequest extends Structure implements Structure.ByReference {
    public int pluginIndex;
    public String context;

    @Override
    protected List<String> getFieldOrder() {
        return Arrays.asList("pluginIndex", "context");
    }
}
