package com.helospark.tactview.openfx.nativerequest;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Structure;

public class ParameterList extends Structure implements Structure.ByReference {
    public int numberOfParameters;
    public Parameter parameter;

    @Override
    protected List<String> getFieldOrder() {
        return Arrays.asList("numberOfParameters", "parameter");
    }
}