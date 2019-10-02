package com.helospark.tactview.openfx;

public class OpenfxClip {
    private String name;
    private boolean isMask;

    public OpenfxClip(String name, boolean isMask) {
        this.name = name;
        this.isMask = isMask;
    }

    public String getName() {
        return name;
    }

    public boolean isMask() {
        return isMask;
    }

}
