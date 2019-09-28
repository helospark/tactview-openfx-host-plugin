package com.helospark.tactview.openfx;

import java.util.Collections;
import java.util.List;

import javax.annotation.Generated;

import com.helospark.lightdi.annotation.Component;
import com.helospark.tactview.core.repository.ProjectRepository;

class LoadPluginResult {
    public int pluginIndex;
    public List<String> supportedContexts;
    public String name;

    @Generated("SparkTools")
    private LoadPluginResult(Builder builder) {
        this.pluginIndex = builder.pluginIndex;
        this.supportedContexts = builder.supportedContexts;
        this.name = builder.name;
    }

    @Generated("SparkTools")
    public static Builder builder() {
        return new Builder();
    }

    @Generated("SparkTools")
    public static final class Builder {
        private int pluginIndex;
        private List<String> supportedContexts = Collections.emptyList();
        private String name;

        private Builder() {
        }

        public Builder withPluginIndex(int pluginIndex) {
            this.pluginIndex = pluginIndex;
            return this;
        }

        public Builder withSupportedContexts(List<String> supportedContexts) {
            this.supportedContexts = supportedContexts;
            return this;
        }

        public Builder withName(String name) {
            this.name = name;
            return this;
        }

        public LoadPluginResult build() {
            return new LoadPluginResult(this);
        }
    }
}

@Component
public class LoadPluginService {
    private ProjectRepository projectRepository;

    public LoadPluginService(ProjectRepository projectRepository) {
        this.projectRepository = projectRepository;
    }

    public LoadPluginResult loadPlugin(int libraryIndex, int i) {
        return null;
        //
        //        DescribeRequest describeRequest = new DescribeRequest();
        //        describeRequest.pluginIndex = pluginIndex;
        //
        //        OpenfxLibrary.INSTANCE.describe(describeRequest);
        //
        //        //            System.out.println("name=" + describeRequest.name + "\n description=" + describeRequest.description);
        //        List<String> supportedContexts = Arrays.asList(describeRequest.supportedContexts.getStringArray(0, describeRequest.supportedContextSize));
        //
        //        return LoadPluginResult.builder()
        //                .withName(describeRequest.name)
        //                .withPluginIndex(pluginIndex)
        //                .withSupportedContexts(supportedContexts)
        //                .build();
    }

}
