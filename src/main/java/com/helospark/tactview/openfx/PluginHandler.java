package com.helospark.tactview.openfx;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import javax.imageio.ImageIO;

import com.helospark.tactview.openfx.nativerequest.CreateInstanceRequest;
import com.helospark.tactview.openfx.nativerequest.DescribeInContextRequest;
import com.helospark.tactview.openfx.nativerequest.DescribeRequest;
import com.helospark.tactview.openfx.nativerequest.InitializeHostRequest;
import com.helospark.tactview.openfx.nativerequest.LoadImageImplementation;
import com.helospark.tactview.openfx.nativerequest.LoadLibraryRequest;
import com.helospark.tactview.openfx.nativerequest.LoadPluginRequest;
import com.helospark.tactview.openfx.nativerequest.Parameter;
import com.helospark.tactview.openfx.nativerequest.ParameterList;
import com.helospark.tactview.openfx.nativerequest.ParameterMap;
import com.helospark.tactview.openfx.nativerequest.RenderImageRequest;

public class PluginHandler {
    static BufferedImage img;
    static ByteBuffer imageData = null;

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
        initializeHostRequest.loadImageCallback = new LoadImageImplementation(img.getWidth(), img.getHeight(), imageData);
        OpenfxLibrary.INSTANCE.initializeHost(initializeHostRequest);

        LoadLibraryRequest loadLibraryRequest = new LoadLibraryRequest();
        loadLibraryRequest.file = "/home/black/tmp/openfx-misc/Misc/Linux-64-debug/Misc.ofx";
        int libraryIndex = OpenfxLibrary.INSTANCE.loadLibrary(loadLibraryRequest);

        LoadPluginRequest request = new LoadPluginRequest();
        request.width = img.getWidth();
        request.height = img.getHeight();
        request.pluginIndex = 2;
        request.libraryDescriptor = libraryIndex;;

        int pluginIndex = OpenfxLibrary.INSTANCE.loadPlugin(request);

        DescribeRequest describeRequest = new DescribeRequest();

        OpenfxLibrary.INSTANCE.describe(describeRequest);

        System.out.println("name=" + describeRequest.name + "\n description=" + describeRequest.description);
        String[] supportedContexts = describeRequest.supportedContexts.getStringArray(0, describeRequest.supportedContextSize);
        System.out.println("Supported Contexts:");
        for (String context : supportedContexts) {
            System.out.println(" - " + context);
        }

        DescribeInContextRequest describeInContextRequest = new DescribeInContextRequest();
        describeInContextRequest.list = new ParameterList();

        OpenfxLibrary.INSTANCE.describeInContext(describeInContextRequest);

        CreateInstanceRequest createInstanceRequest = new CreateInstanceRequest();
        createInstanceRequest.width = request.width;
        createInstanceRequest.height = request.height;
        OpenfxLibrary.INSTANCE.createInstance(createInstanceRequest);

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

        OpenfxLibrary.INSTANCE.closePlugin(pluginIndex, libraryIndex);
    }

    private static void renderFrame(int pluginIndex, int width, int height, double time) throws IOException {
        ByteBuffer returnValue = ByteBuffer.allocateDirect(img.getWidth() * img.getHeight() * 4);
        RenderImageRequest renderImageRequest = new RenderImageRequest();
        renderImageRequest.width = width;
        renderImageRequest.height = height;
        renderImageRequest.time = 0.0;
        renderImageRequest.pluginIndex = pluginIndex;
        renderImageRequest.returnValue = returnValue;
        renderImageRequest.inputImage = imageData;

        OpenfxLibrary.INSTANCE.renderImage(renderImageRequest);

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
