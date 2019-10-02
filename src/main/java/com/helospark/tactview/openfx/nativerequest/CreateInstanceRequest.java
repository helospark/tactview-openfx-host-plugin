package com.helospark.tactview.openfx.nativerequest;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Structure;

public class CreateInstanceRequest extends Structure implements Structure.ByReference {
    public int pluginIndex;
    public int width;
    public int height;
    public String effectId;
    public ParameterList list;
    public ClipList clips;

    @Override
    protected List<String> getFieldOrder() {
        return Arrays.asList("pluginIndex", "width", "height", "effectId", "list", "clips");
    }
}
