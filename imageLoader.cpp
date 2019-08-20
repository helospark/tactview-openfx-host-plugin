#include <iostream>
#include <fstream>
#include "imageLoader.h"

int readInt(std::ifstream& file) {
    while (file.is_open()) {
        std::string data;
        file >> data;
        //std::cout << "Peaked " << data << std::endl;
        if (data[0] == '#') {
            std::string line;
            getline( file, line );
            std::cout << "Comment read" << line << std::endl;
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

    std::cout << "Read data " << data << std::endl; 

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

        std::cout << "Loadedd image " << width << " " << height << std::endl;

        return new Image(width, height, data);
    } else {
        std::cout << "Wrong format" << std::endl;
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

void writeImage(const char* fileName, Image* image) {
    std::ofstream file(fileName);

    file << "P3\n";
    file << image->width << " " << image->height << " 255 255 255\n";

    for (int i = 0; i < image->height; ++i) {
        for (int j = 0; j < image->width; ++j) {
            file << charToUnsignedInt(image->data[i * image->width * 4 + j * 4 + 0]) << " ";
            file << charToUnsignedInt(image->data[i * image->width * 4 + j * 4 + 1]) << " ";
            file << charToUnsignedInt(image->data[i * image->width * 4 + j * 4 + 2]) << " ";
            file << "\n";
        }
    }
}