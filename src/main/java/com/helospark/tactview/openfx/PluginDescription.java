package com.helospark.tactview.openfx;

import java.io.File;

import javax.annotation.Generated;

public class PluginDescription {
    private String pluginId;
    private String description;
    private File libraryPath;

    @Generated("SparkTools")
    private PluginDescription(Builder builder) {
        this.pluginId = builder.pluginId;
        this.description = builder.description;
        this.libraryPath = builder.libraryPath;
    }

    public String getPluginId() {
        return pluginId;
    }

    public String getDescription() {
        return description;
    }

    public File getLibraryPath() {
        return libraryPath;
    }

    @Generated("SparkTools")
    public static Builder builder() {
        return new Builder();
    }

    @Generated("SparkTools")
    public static final class Builder {
        private String pluginId;
        private String description;
        private File libraryPath;

        private Builder() {
        }

        public Builder withPluginId(String pluginId) {
            this.pluginId = pluginId;
            return this;
        }

        public Builder withDescription(String description) {
            this.description = description;
            return this;
        }

        public Builder withLibraryPath(File libraryPath) {
            this.libraryPath = libraryPath;
            return this;
        }

        public PluginDescription build() {
            return new PluginDescription(this);
        }
    }

}
