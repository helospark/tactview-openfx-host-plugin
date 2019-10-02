package com.helospark.tactview.openfx.nativerequest;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Structure;

public class ClipInformation extends Structure implements Structure.ByReference {
    public String name;
    public int isMask;

    @Override
    protected List<String> getFieldOrder() {
        return Arrays.asList("name", "isMask");
    }

}
