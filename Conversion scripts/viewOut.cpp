#include <fstream>
#include <iostream>
#include <vector>

bool equal(std::vector<unsigned char> first, std::vector<unsigned char> second)
{
    for(int i=0; i< first.size(); i++)
    {
        if (first[i] != second[i])
        {
            return false;
        }
    }
    return true;
}

int main()
{
    std::ifstream newFile("test.png", std::ios::binary);


    if(!newFile)
    {
        printf("Nothing");
    }
    unsigned char byte;

    newFile.seekg(0, std::ios::end); // Move get pointer to the end
    long fileSize = newFile.tellg();   // Get the current position (file size)
    newFile.seekg(0, std::ios::beg);
    
    std::vector<unsigned char> forVec;
    std::vector<unsigned char> whileVec;

    for(int i=0; i < fileSize; i++)
    {
        newFile.read(reinterpret_cast<char*>(&byte), 1);
        forVec.push_back(byte);
        printf("%02X", byte);
    }
    newFile.seekg(0, std::ios::beg);
    while(newFile.read(reinterpret_cast<char*>(&byte), 1))
    {
        whileVec.push_back(byte);
        printf("%02X", byte);
    }
    if(equal(forVec,whileVec))
    {
        printf("Both Work");
    }
    else{printf("fail");}
    
    std::cin.get();

    newFile.close();
    return 0;
}