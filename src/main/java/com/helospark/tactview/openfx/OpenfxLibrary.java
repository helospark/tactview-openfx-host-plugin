package com.helospark.tactview.openfx;

import com.helospark.tactview.openfx.nativerequest.CreateInstanceRequest;
import com.helospark.tactview.openfx.nativerequest.DescribeInContextRequest;
import com.helospark.tactview.openfx.nativerequest.DescribeRequest;
import com.helospark.tactview.openfx.nativerequest.InitializeHostRequest;
import com.helospark.tactview.openfx.nativerequest.LoadLibraryRequest;
import com.helospark.tactview.openfx.nativerequest.LoadPluginRequest;
import com.helospark.tactview.openfx.nativerequest.RenderImageRequest;
import com.sun.jna.Library;
import com.sun.jna.Native;

public interface OpenfxLibrary extends Library {
    OpenfxLibrary INSTANCE = Native.loadLibrary("openfxplugin",
            OpenfxLibrary.class);

    public void initializeHost(InitializeHostRequest initializeHostRequest);

    public int loadLibrary(LoadLibraryRequest loadLibraryRequest);

    public int loadPlugin(LoadPluginRequest loadPluginRequest);

    public int createInstance(CreateInstanceRequest createInstanceRequest);

    public void describe(DescribeRequest describeRequest);

    public void describeInContext(DescribeInContextRequest request);

    int renderImage(RenderImageRequest imageRequest);

    void closePlugin(int pluginIndex, int libraryIndex);
}
