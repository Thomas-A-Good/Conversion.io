#include <iostream>
#include <string>
#include <cstdlib>   // for system()
#include <filesystem>

int main() {
    std::string inputFile;
    std::cout << "Enter WebP file name: ";
    std::cin >> inputFile;

    // Check if file exists
    if (!std::filesystem::exists(inputFile)) {
        std::cerr << "File not found: " << inputFile << "\n";
        return 1;
    }

    // Output file
    std::string outputFile = "output.png";

    // Build the dwebp command
    std::string command =
        "dwebp \"" + inputFile + "\" -o \"" + outputFile + "\"";

    std::cout << "Running: " << command << "\n";

    // Execute the command
    int result = system(command.c_str());

    if (result != 0) {
        std::cerr << "Failed to convert WebP to PNG (dwebp returned code "
                  << result << ")\n";
        return 1;
    }

    std::cout << "Saved PNG as " << outputFile << "\n";
    return 0;
}
