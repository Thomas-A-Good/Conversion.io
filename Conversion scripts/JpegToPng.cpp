#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <string>

int main() {
    std::string inputFile;
    std::cout << "Enter JPEG file name: ";
    std::cin >> inputFile;

    int width, height, channels;
    // Load JPEG image (channels will be detected automatically)
    unsigned char *img = stbi_load(inputFile.c_str(), &width, &height, &channels, 0);
    if (!img) {
        std::cerr << "Failed to load image: " << inputFile << "\n";
        return 1;
    }

    std::string outputFile = "output.png";
    int stride = width * channels; // bytes per row

    // Save as PNG
    if (stbi_write_png(outputFile.c_str(), width, height, channels, img, stride))
        std::cout << "Saved PNG as " << outputFile << "\n";
    else
        std::cerr << "Failed to save PNG\n";

    stbi_image_free(img); // free memory
    return 0;
}