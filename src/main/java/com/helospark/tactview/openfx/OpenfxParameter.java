package com.helospark.tactview.openfx;

import java.util.Collections;
import java.util.List;
import java.util.Map;

import javax.annotation.Generated;

public class OpenfxParameter {
    private String name;
    private String type;
    private int uniqueParameterId;
    private Map<String, List<String>> metadata;

    @Generated("SparkTools")
    private OpenfxParameter(Builder builder) {
        this.name = builder.name;
        this.type = builder.type;
        this.uniqueParameterId = builder.uniqueParameterId;
        this.metadata = builder.metadata;
    }

    public String getName() {
        return name;
    }

    public int getUniqueParameterId() {
        return uniqueParameterId;
    }

    public String getType() {
        return type;
    }

    public Map<String, List<String>> getMetadata() {
        return metadata;
    }

    @Generated("SparkTools")
    public static Builder builder() {
        return new Builder();
    }

    @Generated("SparkTools")
    public static final class Builder {
        private String name;
        private String type;
        private int uniqueParameterId;
        private Map<String, List<String>> metadata = Collections.emptyMap();

        private Builder() {
        }

        public Builder withName(String name) {
            this.name = name;
            return this;
        }

        public Builder withType(String type) {
            this.type = type;
            return this;
        }

        public Builder withUniqueParameterId(int uniqueParameterId) {
            this.uniqueParameterId = uniqueParameterId;
            return this;
        }

        public Builder withMetadata(Map<String, List<String>> metadata) {
            this.metadata = metadata;
            return this;
        }

        public OpenfxParameter build() {
            return new OpenfxParameter(this);
        }
    }

}
