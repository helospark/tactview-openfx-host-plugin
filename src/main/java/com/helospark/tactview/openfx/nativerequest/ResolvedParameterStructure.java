package com.helospark.tactview.openfx.nativerequest;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Structure;

public class ResolvedParameterStructure extends Structure implements Structure.ByReference {
    public int intValue1;
    public int intValue2;
    public int intValue3;

    public double doubleValue1;
    public double doubleValue2;
    public double doubleValue3;
    public double doubleValue4;

    @Override
    protected List<String> getFieldOrder() {
        return Arrays.asList("intValue1", "intValue2", "intValue3", "doubleValue1", "doubleValue2", "doubleValue3", "doubleValue4");
    }
}
