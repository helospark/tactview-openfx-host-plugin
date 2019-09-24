package com.helospark.tactview.openfx.nativerequest;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Structure;

public class InitializeHostRequest extends Structure implements Structure.ByReference {
    public LoadImageCallback loadImageCallback;
    public ParameterValueProviderCallback parameterValueProviderCallback;

    @Override
    protected List<String> getFieldOrder() {
        return Arrays.asList("loadImageCallback", "parameterValueProviderCallback");
    }

}
