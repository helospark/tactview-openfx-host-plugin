#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "imageLoader.h"
#include "global.h"

int readInt(std::ifstream& file) {
    while (file.is_open()) {
        std::string data;
        file >> data;
        //LOG("Peaked " << data );
        if (data[0] == '#') {
            std::string line;
            getline( file, line );
            LOG("Comment read" << line );
        } else {
            return std::stoi(data);
        }
    }
    return -1;
}

Image* loadImage(const char* fileName) {
    std::ifstream file(fileName);
    std::string data;

    file >> data;

    LOG("Read data " << data ); 

    if (data == "P3") {
        int width  = readInt(file);
        int height  = readInt(file);

        char* data = new char[width * height * 4];

        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                for (int k = 0; k < 3; ++k) {
                    int pixel = readInt(file);

                    data[i * width * 4 + j * 4 + k] = (char)pixel;
                }
                data[i * width * 4 + j * 4 + 3] = (char)255;
            }
        }

        LOG("Loadedd image " << width << " " << height );

        return new Image(width, height, data);
    } else {
        LOG("Wrong format" );
        return NULL;
    }

}

int charToUnsignedInt(char data) {
    int iData = (int)data;
    if (data < 0) {
        return iData + 256;
    } else {
        return iData;
    }
}

int clamp(int v, int min, int max) {
    if (v < min) {
        return min;
    }
    if (v > max) {
        return max;
    }
    return v;
}

void writeImage(const char* fileName, Image* image, char* type) {
    std::ofstream file(fileName);

    file << "P3\n";
    file << image->width << " " << image->height << " 255 255 255\n";

    if (strcmp(type, "OfxBitDepthByte") == 0) {
        char* imageData = (char*)image->data;
        for (int i = 0; i < image->height; ++i) {
            for (int j = 0; j < image->width; ++j) {
                file << charToUnsignedInt(imageData[i * image->width * 4 + j * 4 + 0]) << " ";
                file << charToUnsignedInt(imageData[i * image->width * 4 + j * 4 + 1]) << " ";
                file << charToUnsignedInt(imageData[i * image->width * 4 + j * 4 + 2]) << " ";
                file << "\n";
            }
        }
    } else {
        float* imageData = (float*)image->data;
        for (int i = 0; i < image->height; ++i) {
            for (int j = 0; j < image->width; ++j) {
                file << clamp((int)(imageData[i * image->width * 4 + j * 4 + 0] * 255.0f), 0, 255) << " ";
                file << clamp((int)(imageData[i * image->width * 4 + j * 4 + 1] * 255.0f), 0, 255) << " ";
                file << clamp((int)(imageData[i * image->width * 4 + j * 4 + 2] * 255.0f), 0, 255) << " ";
                file << "\n";
            }
        }
    }
}