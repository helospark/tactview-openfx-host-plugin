package com.helospark.tactview.openfx.nativerequest;

import java.util.List;

import com.sun.jna.Structure;

public class LoadLibraryRequest extends Structure implements Structure.ByReference {
    public String file;

    public int numberOfPlugins;

    @Override
    protected List<String> getFieldOrder() {
        return List.of("file", "numberOfPlugins");
    }
}
