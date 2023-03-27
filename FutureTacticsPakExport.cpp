#include <fstream>
#include <iostream>
#include <filesystem>


struct FileHeader {
    char name[48];
    uint32_t pos;
    struct {
        uint32_t size : 24;
        uint32_t flag : 8;
    } sf;
};

struct PakHeader {
    uint32_t fileHeaderCount_;
    FileHeader* fileHeaders_;
};

int main() {
    // Open the file "files.pak" from current directory
    std::ifstream file("files.pak", std::ios::binary);

    // Get the file size
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();

    // Seek back to the beginning
    file.seekg(0, std::ios::beg);


    // Read the number of file headers
    PakHeader pakHeader;
    file.read(reinterpret_cast<char*>(&pakHeader.fileHeaderCount_), sizeof(uint32_t));

    // Allocate memory for the file headers
    pakHeader.fileHeaders_ = new FileHeader[pakHeader.fileHeaderCount_];

    // Read the file headers
    file.read(reinterpret_cast<char*>(pakHeader.fileHeaders_), pakHeader.fileHeaderCount_ * sizeof(FileHeader));

    // Create a folder named "export"
    std::string exportFolder = "export";
    std::filesystem::create_directory(exportFolder);

    // Export the files
    for (uint32_t i = 0; i < pakHeader.fileHeaderCount_; ++i) {

        // Print the position and size of the file
        std::cout << "Pos: " << std::hex << pakHeader.fileHeaders_[i].pos << " Size: " << std::dec << pakHeader.fileHeaders_[i].sf.size << std::endl;

        // Seek to the file position
        file.seekg(pakHeader.fileHeaders_[i].pos);

        // Read the file data
        char* fileData = new char[pakHeader.fileHeaders_[i].sf.size];
        file.read(fileData, pakHeader.fileHeaders_[i].sf.size);

        // Print 4 first bytes as characters (Funny console beeps :>)
        // std::cout << "First 4 bytes: " << fileData[0] << fileData[1] << fileData[2] << fileData[3] << std::endl;

        // Sometimes the file name contains folders, create them now
        std::filesystem::create_directories(exportFolder + "/" + std::filesystem::path(pakHeader.fileHeaders_[i].name).parent_path().string());

        // Get the extension of the name (lowercased)
        std::string extension = std::filesystem::path(pakHeader.fileHeaders_[i].name).extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

       

        // Create the file
        std::ofstream outFile(exportFolder + "/" + pakHeader.fileHeaders_[i].name, std::ios::binary);
        outFile.write(fileData, pakHeader.fileHeaders_[i].sf.size);

        outFile.close();

        // Free the memory for the file data
        delete[] fileData;
        fileData = nullptr;

        std::cout << "Exported " << pakHeader.fileHeaders_[i].name << std::endl;
    }

    // Free the memory for the file headers
    delete[] pakHeader.fileHeaders_;
    pakHeader.fileHeaders_ = nullptr;

    std::cout << "Exported " << pakHeader.fileHeaderCount_ << " files" << std::endl;


    return 0;
}
