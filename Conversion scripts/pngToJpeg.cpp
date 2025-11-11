#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdint>
#include <zlib.h>

std::vector<unsigned char> decompressIDAT(const std::vector<unsigned char> &IDAT, uint32_t width, uint32_t height, uint8_t channels)
{
    uLongf decompressedSize = height * (1 + width * channels); // +1 for filter byte per scanline
    std::vector<unsigned char> decompressedData(decompressedSize);

    int ret = uncompress(decompressedData.data(), &decompressedSize, IDAT.data(), IDAT.size());
    if (ret != Z_OK)
    {
        throw std::runtime_error("Failed to decompress IDAT data");
    }
    decompressedData.resize(decompressedSize);
    return decompressedData;
}

unsigned char paeth(int a, int b, int c)
{
    int p = a + b - c;
    int pa = abs(p - a);
    int pb = abs(p - b);
    int pc = abs(p - c);
    if (pa <= pb && pa <= pc) return a;
    else if (pb <= pc) return b;
    else return c;
}

std::vector<unsigned char> reconstructPixels(const std::vector<unsigned char> &decompressedData, uint32_t width, uint32_t height, uint8_t channels)
{
    std::vector<unsigned char> pixels(width * height * channels);
    size_t rowBytes = width * channels;

    for(uint32_t y = 0; y < height; y++)
    {
        size_t rowStart = y * (1 + rowBytes);
        uint8_t filterType = decompressedData[rowStart];

        for(size_t x = 0; x < rowBytes; x++)
        {
            unsigned char raw = decompressedData[rowStart + 1 + x];
            unsigned char left = (x >= channels) ? pixels[y * rowBytes + x - channels] : 0;
            unsigned char up = (y > 0) ? pixels[(y-1) * rowBytes + x] : 0;
            unsigned char upLeft = (y > 0 && x >= channels) ? pixels[(y-1) * rowBytes + x - channels] : 0;

            unsigned char val = 0;
            switch(filterType)
            {
                case 0: val = raw; break; // None
                case 1: val = raw + left; break; // Sub
                case 2: val = raw + up; break; // Up
                case 3: val = raw + ((left + up)/2); break; // Average
                case 4: val = raw + paeth(left, up, upLeft); break; // Paeth
                default: throw std::runtime_error("Unknown filter type");
            }

            pixels[y * rowBytes + x] = val;
        }
    }

    return pixels;
}

uint32_t readLength(std::ifstream &pngFile)
{
    unsigned char lengthBytes[4];
    pngFile.read(reinterpret_cast<char*>(lengthBytes), 4);
    
    return (lengthBytes[0] << 24) | (lengthBytes[1] << 16) | (lengthBytes[2] << 8) | (lengthBytes[3]);
}

std::string readType(std::ifstream &pngFile)
{
    char type[5] = {0};
    pngFile.read(type, 4);

    return  std::string(type);
}

void readChunk(std::ifstream &pngFile, std::vector<unsigned char> &IHDR, std::vector<unsigned char> &IDAT, std::vector<unsigned char> &PLTE)
{
    uint32_t chunkLength = readLength(pngFile);
    std::string chunkType = readType(pngFile);

    std::vector <unsigned char> data(chunkLength);
    pngFile.read(reinterpret_cast<char*>(data.data()), chunkLength); 

    unsigned char crcBytes[4];
    pngFile.read(reinterpret_cast<char*>(crcBytes), 4);

    if (chunkType == "IDAT")
    {
        IDAT.insert(IDAT.end(), data.begin(), data.end());
    }
    else if(chunkType == "IHDR")
    {
        IHDR = data;
    }
    else if (chunkType == "PLTE")
    {
        PLTE = data;
    } 
    else if(chunkType == "IEND")
    {
        std::cout << "End of chunk";
        return;
    }
    else{
        std::cout << "Chunk skipped";
    }
    std::cout << "Chunk: " << chunkType << " (" << chunkLength << " bytes)\n";
}

int main()
{
    unsigned char blankVar;
    /*
    std::vector<unsigned char> bytes;
    unsigned char byte;
    */
    std::vector<unsigned char> signature;
    std::vector<unsigned char> IHDR; 
    std::vector<unsigned char> IDAT;
    std::vector<unsigned char> palette;

    std::string fileName;

    std::cout << "Enter file: ";
    std::cin >> fileName;
    
    std::ifstream pngFile(fileName, std::ios::binary);

    signature.resize(8);
    pngFile.read(reinterpret_cast<char*>(signature.data()), 8);
    
    while (pngFile && !pngFile.eof()) {
        readChunk(pngFile, IHDR, IDAT, palette);
    }
    pngFile.close();

    std::cout << "\nIHDR size: " << IHDR.size() << " bytes";
    std::cout << "\nTotal IDAT data: " << IDAT.size() << " bytes\n";

    uint32_t width  = (IHDR[0] << 24) | (IHDR[1] << 16) | (IHDR[2] << 8) | IHDR[3];
    uint32_t height = (IHDR[4] << 24) | (IHDR[5] << 16) | (IHDR[6] << 8) | IHDR[7];
    uint8_t bitDepth = IHDR[8];
    uint8_t colorType = IHDR[9];

    std::cout << "Width: " << width << "\n";
    std::cout << "Height: " << height << "\n";
    std::cout << "Bit depth: " << (int)bitDepth << "\n";
    std::cout << "Color type: " << (int)colorType << "\n";

    uint8_t channels;
    if (colorType == 2) channels = 3;    // RGB
    else if (colorType == 6) channels = 4; // RGBA
    else if (colorType == 3) channels = 1; // palette index
    else throw std::runtime_error("Unsupported color type");

    auto decompressed = decompressIDAT(IDAT, width, height, channels);
    std::cout << "Expected size: " << height * (1 + width * channels) << " Decompressed size: " << decompressed.size() << "\n";
    auto pixels = reconstructPixels(decompressed, width, height, channels);

    if (colorType == 3) {
    // each pixel is an index into the palette
    std::vector<unsigned char> rgb(width * height * 3);
    for (size_t i = 0; i < width * height; ++i) {
        unsigned char idx = pixels[i];
        rgb[i * 3 + 0] = palette[idx * 3 + 0];
        rgb[i * 3 + 1] = palette[idx * 3 + 1];
        rgb[i * 3 + 2] = palette[idx * 3 + 2];
    }
    pixels.swap(rgb);
    channels = 3;
}

    std::cout << "Pixel data size: " << pixels.size() << "\n";

    std::cout << "Pixel data size: " << pixels.size() << "\n";

// Save as JPEG
    if (stbi_write_jpg("output.jpg", width, height, channels, pixels.data(), 100))
        std::cout << "Saved as output.jpg\n";
    else{
        std::cerr << "Failed to save JPEG\n";
    }    

    std::cout << "colorType = " << (int)colorType << "\n";

    uint8_t interlaceMethod = IHDR[12]; // IHDR[12] is interlace method
    std::cout << "Interlace method: " << (int)interlaceMethod << "\n";
    std::cin >> blankVar;

    return 0;
}
