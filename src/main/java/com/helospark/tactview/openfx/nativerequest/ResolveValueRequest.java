package com.helospark.tactview.openfx.nativerequest;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Structure;

public class ResolveValueRequest extends Structure implements Structure.ByReference {
    public int uniqueId;
    public double time;

    public ResolvedParameterStructure result;

    @Override
    protected List<String> getFieldOrder() {
        return Arrays.asList("uniqueId", "time", "result");
    }
}
