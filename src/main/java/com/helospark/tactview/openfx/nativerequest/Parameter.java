package com.helospark.tactview.openfx.nativerequest;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Structure;

public class Parameter extends Structure implements Structure.ByReference {
    public int numberOfEntries;
    public ParameterMap parameterMap;
    public String name;
    public String type;

    @Override
    protected List<String> getFieldOrder() {
        return Arrays.asList("numberOfEntries", "parameterMap", "name", "type");
    }
}
