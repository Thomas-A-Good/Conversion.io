#include <iostream>
#include <string>
#include <cstdlib>   // for system()
#include <filesystem>

int main() {
    std::string inputFile;
    std::cout << "Enter PNG file name: ";
    std::cin >> inputFile;

    // Check if file exists
    if (!std::filesystem::exists(inputFile)) {
        std::cerr << "File not found: " << inputFile << "\n";
        return 1;
    }

    // Output file
    std::string outputFile = "output.webp";
    int quality = 100;

    // Build the cwebp command
    std::string command =
        "cwebp -q " + std::to_string(quality) + " \"" + inputFile + "\" -o \"" + outputFile + "\"";

    std::cout << "Running: " << command << "\n";

    // Execute the command
    int result = system(command.c_str());

    if (result != 0) {
        std::cerr << "Failed to convert image (cwebp returned code " << result << ")\n";
        return 1;
    }

    std::cout << "Saved WebP as " << outputFile << "\n";
    return 0;
}
