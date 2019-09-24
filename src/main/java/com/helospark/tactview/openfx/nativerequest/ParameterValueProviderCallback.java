package com.helospark.tactview.openfx.nativerequest;

import com.sun.jna.Callback;

public interface ParameterValueProviderCallback extends Callback {

    public void resolveValue(ResolveValueRequest resolveValueRequest);

}
