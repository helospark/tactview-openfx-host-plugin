package com.helospark.tactview.openfx;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;
import java.util.List;

import javax.imageio.ImageIO;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Structure;

public class PluginHandler {
    static BufferedImage img;
    static ByteBuffer imageData = null;

    public static class InitializeHostRequest extends Structure implements Structure.ByReference {
        public LoadImageCallback loadImageCallback;

        @Override
        protected List<String> getFieldOrder() {
            return Arrays.asList("loadImageCallback");
        }
    }

    public interface CLibrary extends Library {
        CLibrary INSTANCE = Native.loadLibrary("openfxplugin",
                CLibrary.class);

        public void initializeHost(InitializeHostRequest initializeHostRequest);

        public int loadPlugin(LoadPluginRequest loadPluginRequest);

        int renderImage(RenderImageRequest imageRequest);

        void closePlugin(int pluginIndex);
    }

    public static class LoadImageRequest extends Structure implements Structure.ByReference {
        public double time;
        public int width;
        public int height;
        public ByteBuffer data;

        @Override
        protected List<String> getFieldOrder() {
            return Arrays.asList("time", "width", "height", "data");
        }
    }

    public static class LoadImageImplementation implements LoadImageCallback {

        @Override
        public void loadImage(LoadImageRequest loadImageRequest) {
            System.out.println("Loading image at time " + loadImageRequest.time);

            loadImageRequest.width = img.getWidth();
            loadImageRequest.height = img.getHeight();
            loadImageRequest.data = imageData;
        }

    }

    public static void main(String[] args) throws IOException {
        img = ImageIO.read(new File("/home/black/Downloads/image_j.jpg"));

        imageData = java.nio.ByteBuffer.allocateDirect(img.getWidth() * img.getHeight() * 4);
        imageData.order(ByteOrder.nativeOrder());

        for (int i = 0; i < img.getHeight(); ++i) {
            for (int j = 0; j < img.getWidth(); ++j) {
                Color pixel = new Color(img.getRGB(j, i));
                imageData.put((byte) pixel.getBlue());
                imageData.put((byte) pixel.getRed());
                imageData.put((byte) pixel.getGreen());
                imageData.put((byte) 255);
            }
        }

        InitializeHostRequest initializeHostRequest = new InitializeHostRequest();
        initializeHostRequest.loadImageCallback = new LoadImageImplementation();

        CLibrary.INSTANCE.initializeHost(initializeHostRequest);

        LoadPluginRequest request = new LoadPluginRequest();
        request.width = img.getWidth();
        request.height = img.getHeight();

        int pluginIndex = CLibrary.INSTANCE.loadPlugin(request);

        RenderImageRequest renderImageRequest = new RenderImageRequest();
        renderImageRequest.width = request.width;
        renderImageRequest.height = request.height;
        renderImageRequest.time = 0.0;
        renderImageRequest.pluginIndex = pluginIndex;

        CLibrary.INSTANCE.renderImage(renderImageRequest);

        CLibrary.INSTANCE.closePlugin(pluginIndex);
    }

}
