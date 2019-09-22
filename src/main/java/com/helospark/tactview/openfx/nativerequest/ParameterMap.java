package com.helospark.tactview.openfx.nativerequest;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;

public class ParameterMap extends Structure implements Structure.ByReference {
    public int numberOfValues;
    public String key;
    public Pointer value;

    @Override
    protected List<String> getFieldOrder() {
        return Arrays.asList("numberOfValues", "key", "value");
    }
}
