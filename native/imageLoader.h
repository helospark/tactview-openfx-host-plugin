#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H


struct Image {
    int width, height;
    void* data;

    Image(int width2, int height2, void* data2) : width(width2), height(height2), data(data2) {}

    ~Image() {
        delete[] data;
    }
};

Image* loadImage(const char* imageFileName);

void writeImage(const char* imageFileName, Image* image, char* type);

#endif