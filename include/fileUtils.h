#pragma once

#include <filesystem> 

namespace fs = std::filesystem;

inline bool createDirectory(const std::string& dirName) {
    try {
        if (!fs::exists(dirName)) {
            return fs::create_directory(dirName);
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating directory " << dirName << ": " << e.what() << std::endl;
        return false;
    }
}

inline bool removeDirectory(const std::string& dirName) {
    try {
        if (fs::exists(dirName)) {
            return fs::remove_all(dirName) > 0;  
        }
        return false; 
    } catch (const std::exception& e) {
        std::cerr << "Error removing directory " << dirName << ": " << e.what() << std::endl;
        return false;
    }
}

inline bool createFile(const std::string& fileName) {
    try {
        if (!fs::exists(fileName)) {
            std::ofstream file(fileName);
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error creating file " << fileName << ": " << e.what() << std::endl;
        return false;
    }
}

inline bool removeFile(const std::string& fileName) {
    try {
        if (fs::exists(fileName)) {
            return fs::remove(fileName);
        }
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error removing file " << fileName << ": " << e.what() << std::endl;
        return false;
    }
}