package com.helospark.tactview.openfx;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Optional;
import java.util.Set;

import org.apache.commons.lang3.SystemUtils;

import com.helospark.lightdi.annotation.Component;
import com.helospark.lightdi.annotation.Value;

@Component
public class OpenFXPluginPathProvider {
    private String openfxPluginLocations;
    private String platformIdentifier;

    public OpenFXPluginPathProvider(@Value("${openfx.plugin.locations}") String openfxPluginLocations) {
        this.openfxPluginLocations = openfxPluginLocations;

        boolean isx86 = Optional.ofNullable(System.getenv("os.arch")).map(a -> a.equals("x86")).orElse(false);

        if (SystemUtils.IS_OS_LINUX) {
            if (isx86) {
                platformIdentifier = "Linux-x86";
            } else {
                platformIdentifier = "Linux-x86-64";
            }
        } else if (SystemUtils.IS_OS_WINDOWS) {
            if (isx86) {
                platformIdentifier = "Win32";
            } else {
                platformIdentifier = "Win64";
            }
        } else if (SystemUtils.IS_OS_MAC) {
            platformIdentifier = "MacOS";
        } else {
            platformIdentifier = "unknown";
        }
    }

    public Set<File> getOpenFxBundles() {
        String ofxPluginPath = System.getenv("OFX_PLUGIN_PATH");

        Set<File> plugins = new HashSet<>();
        if (SystemUtils.IS_OS_LINUX) {
            plugins.addAll(searchPath("/usr/OFX/Plugins"));
            if (ofxPluginPath != null) {
                Arrays.stream(ofxPluginPath.split(":"))
                        .forEach(a -> plugins.addAll(searchPath(a)));
            }
        } else if (SystemUtils.IS_OS_WINDOWS) {
            plugins.addAll(searchPath("C:\\Program Files\\Common Files\\OFX\\Plugins"));
            if (ofxPluginPath != null) {
                Arrays.stream(ofxPluginPath.split(";"))
                        .forEach(a -> plugins.addAll(searchPath(a)));
            }
        } else if (SystemUtils.IS_OS_MAC) {
            plugins.addAll(searchPath("/Library/OFX/Plugins/"));
            if (ofxPluginPath != null) {
                Arrays.stream(ofxPluginPath.split(":"))
                        .forEach(a -> plugins.addAll(searchPath(a)));
            }
        }

        Arrays.stream(openfxPluginLocations.split(":"))
                .forEach(a -> plugins.addAll(searchPath(a)));

        return plugins;
    }

    private List<File> searchPath(String pathString) {
        List<File> result = new ArrayList<>();
        File path = new File(pathString);
        if (!path.exists()) {
            return Collections.emptyList();
        }
        if (path.isFile() && path.getAbsolutePath().endsWith(".ofx")) {
            return List.of(path);
        }
        if (path.isDirectory()) {
            for (File childFile : path.listFiles()) {
                result.addAll(getBundleFile(childFile));
            }
        }
        return result;
    }

    private List<File> getBundleFile(File childFile) {
        List<File> result = new ArrayList<>();
        File contentsFolder = new File(childFile, "Contents");
        if (contentsFolder.exists()) {
            File pluginFolder = new File(contentsFolder, platformIdentifier);
            if (pluginFolder.exists() && pluginFolder.isDirectory()) {
                for (File ofxPluginFile : pluginFolder.listFiles()) {
                    if (ofxPluginFile.getName().endsWith(".ofx")) {
                        result.add(ofxPluginFile);
                    }
                }
            }
        }
        return result;
    }

}
