#include <iostream>
#include <string>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <webp/encode.h>

int main() {
    std::string inputFile;
    std::cout << "Enter PNG file name: ";
    std::cin >> inputFile;

    int width, height, channels;
    unsigned char* img = stbi_load(inputFile.c_str(), &width, &height, &channels, 0);
    if (!img) {
        std::cerr << "Failed to load image: " << inputFile << "\n";
        return 1;
    }

    std::string outputFile = "output.webp";
    int quality = 100;

    uint8_t* outputData = nullptr;
    size_t outputSize = 0;

    if (channels == 4) {
        outputSize = WebPEncodeRGBA(img, width, height, width * 4, quality, &outputData);
    } else if (channels == 3) {

        outputSize = WebPEncodeRGB(img, width, height, width * 3, quality, &outputData);
    } else {
        std::cerr << "Unsupported number of channels: " << channels << "\n";
        stbi_image_free(img);
        return 1;
    }

    if (outputSize == 0) {
        std::cerr << "WebP encoding failed\n";
        stbi_image_free(img);
        return 1;
    }


    std::ofstream out(outputFile, std::ios::binary);
    out.write(reinterpret_cast<char*>(outputData), outputSize);
    out.close();


    WebPFree(outputData);
    stbi_image_free(img);

    std::cout << "Saved WebP as " << outputFile << "\n";
    return 0;
}
