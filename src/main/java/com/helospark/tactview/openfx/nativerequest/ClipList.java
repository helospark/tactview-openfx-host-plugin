package com.helospark.tactview.openfx.nativerequest;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Structure;

public class ClipList extends Structure implements Structure.ByReference {
    public int numberOfEntries;
    public ClipInformation clip;

    @Override
    protected List<String> getFieldOrder() {
        return Arrays.asList("numberOfEntries", "clip");
    }
}
