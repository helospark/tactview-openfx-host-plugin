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
import com.sun.jna.Pointer;
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

    public static class ParameterMap extends Structure implements Structure.ByReference {
        public int numberOfValues;
        public String key;
        public Pointer value;

        @Override
        protected List<String> getFieldOrder() {
            return Arrays.asList("numberOfValues", "key", "value");
        }
    };

    public static class Parameter extends Structure implements Structure.ByReference {
        public int numberOfEntries;
        public ParameterMap parameterMap;
        public String name;
        public String type;

        @Override
        protected List<String> getFieldOrder() {
            return Arrays.asList("numberOfEntries", "parameterMap", "name", "type");
        }
    };

    public static class ParameterList extends Structure implements Structure.ByReference {
        public int numberOfParameters;
        public Parameter parameter;

        @Override
        protected List<String> getFieldOrder() {
            return Arrays.asList("numberOfParameters", "parameter");
        }
    };

    public static class DescribeInContextRequest extends Structure implements Structure.ByReference {
        public ParameterList list;

        @Override
        protected List<String> getFieldOrder() {
            return Arrays.asList("list");
        }
    }

    public static class CreateInstanceRequest extends Structure implements Structure.ByReference {
        public int width;
        public int height;

        @Override
        protected List<String> getFieldOrder() {
            return Arrays.asList("width", "height");
        }
    }

    public interface CLibrary extends Library {
        CLibrary INSTANCE = Native.loadLibrary("openfxplugin",
                CLibrary.class);

        public void initializeHost(InitializeHostRequest initializeHostRequest);

        public int loadLibrary(LoadLibraryRequest loadLibraryRequest);

        public int loadPlugin(LoadPluginRequest loadPluginRequest);

        public int createInstance(CreateInstanceRequest createInstanceRequest);

        public void describe(DescribeRequest describeRequest);

        public void describeInContext(DescribeInContextRequest request);

        int renderImage(RenderImageRequest imageRequest);

        void closePlugin(int pluginIndex, int libraryIndex);
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

    public static class DescribeRequest extends Structure implements Structure.ByReference {
        public String name;
        public String description;
        public int supportedContextSize;
        public Pointer supportedContexts;

        @Override
        protected List<String> getFieldOrder() {
            return Arrays.asList("name", "description", "supportedContextSize", "supportedContexts");
        }
    };

    public static void main(String[] args) throws IOException {
        img = ImageIO.read(new File("/home/black/Downloads/image_jhalf.jpg"));

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

        LoadLibraryRequest loadLibraryRequest = new LoadLibraryRequest();
        loadLibraryRequest.file = "/home/black/tmp/openfx-misc/Misc/Linux-64-debug/Misc.ofx";
        int libraryIndex = CLibrary.INSTANCE.loadLibrary(loadLibraryRequest);

        LoadPluginRequest request = new LoadPluginRequest();
        request.width = img.getWidth();
        request.height = img.getHeight();
        request.pluginIndex = 2;
        request.libraryDescriptor = libraryIndex;;

        int pluginIndex = CLibrary.INSTANCE.loadPlugin(request);

        DescribeRequest describeRequest = new DescribeRequest();

        CLibrary.INSTANCE.describe(describeRequest);

        System.out.println("name=" + describeRequest.name + "\n description=" + describeRequest.description);
        String[] supportedContexts = describeRequest.supportedContexts.getStringArray(0, describeRequest.supportedContextSize);
        System.out.println("Supported Contexts:");
        for (String context : supportedContexts) {
            System.out.println(" - " + context);
        }

        DescribeInContextRequest describeInContextRequest = new DescribeInContextRequest();
        describeInContextRequest.list = new ParameterList();

        CLibrary.INSTANCE.describeInContext(describeInContextRequest);

        CreateInstanceRequest createInstanceRequest = new CreateInstanceRequest();
        createInstanceRequest.width = request.width;
        createInstanceRequest.height = request.height;
        CLibrary.INSTANCE.createInstance(createInstanceRequest);

        System.out.println("Parameters:");
        Parameter[] parameter = (Parameter[]) describeInContextRequest.list.parameter.toArray(describeInContextRequest.list.numberOfParameters);
        for (int i = 0; i < describeInContextRequest.list.numberOfParameters; ++i) {
            System.out.println(parameter[i].name + "   " + parameter[i].type);
            ParameterMap[] paramMap = (ParameterMap[]) parameter[i].parameterMap.toArray(parameter[i].numberOfEntries);
            for (int j = 0; j < parameter[i].numberOfEntries; ++j) {
                System.out.print("key=" + paramMap[j].key + " : ");
                String[] elements = paramMap[j].value.getStringArray(0, paramMap[j].numberOfValues);
                for (String element : elements) {
                    System.out.print(element + " ");
                }
                System.out.println();
            }
            System.out.println("\n----\n");
        }

        int width = request.width;
        int height = request.height;

        renderFrame(pluginIndex, width, height, 0.0);
        renderFrame(pluginIndex, width, height, 1.0);

        CLibrary.INSTANCE.closePlugin(pluginIndex, libraryIndex);
    }

    private static void renderFrame(int pluginIndex, int width, int height, double time) throws IOException {
        ByteBuffer returnValue = ByteBuffer.allocateDirect(img.getWidth() * img.getHeight() * 4);
        RenderImageRequest renderImageRequest = new RenderImageRequest();
        renderImageRequest.width = width;
        renderImageRequest.height = height;
        renderImageRequest.time = 0.0;
        renderImageRequest.pluginIndex = pluginIndex;
        renderImageRequest.returnValue = returnValue;

        CLibrary.INSTANCE.renderImage(renderImageRequest);

        BufferedImage writableImage = new BufferedImage(width, height, BufferedImage.TYPE_4BYTE_ABGR);

        for (int i = 0; i < img.getHeight(); ++i) {
            for (int j = 0; j < img.getWidth(); ++j) {
                int r = charToUnsignedInt(renderImageRequest.returnValue.get(i * img.getWidth() * 4 + j * 4 + 1));
                int g = charToUnsignedInt(renderImageRequest.returnValue.get(i * img.getWidth() * 4 + j * 4 + 2));
                int b = charToUnsignedInt(renderImageRequest.returnValue.get(i * img.getWidth() * 4 + j * 4 + 0));
                int a = 255;// charToUnsignedInt(renderImageRequest.returnValue.get(i * img.getWidth() * 4 + j * 4 + 3));
                writableImage.setRGB(j, i, new Color(r, g, b, a).getRGB());
            }
        }
        ImageIO.write(writableImage, "png", new File("/tmp/result_" + time + ".png"));
    }

    static int charToUnsignedInt(byte data) {
        int iData = data;
        if (data < 0) {
            return iData + 256;
        } else {
            return iData;
        }
    }

}
