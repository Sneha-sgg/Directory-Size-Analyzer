#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <windows.h>
#include <algorithm>

struct FileInfo {
    std::string name;
    long long size;
};

void analyzeDirectory(const std::string& directoryPath, long long& totalSize, std::map<std::string, std::pair<int, long long>>& fileTypes, std::vector<FileInfo>& largestFiles) {
    WIN32_FIND_DATAA fileData;
    std::string searchPath = directoryPath + "\\*";
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &fileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (strcmp(fileData.cFileName, ".") != 0 && strcmp(fileData.cFileName, "..") != 0) {
                    std::string subDirectoryPath = directoryPath + "\\" + fileData.cFileName;
                    analyzeDirectory(subDirectoryPath, totalSize, fileTypes, largestFiles);
                }
            }
            else {
                std::string filePath = directoryPath + "\\" + fileData.cFileName;
                std::ifstream file(filePath, std::ios::binary | std::ios::ate);
                if (file.is_open()) {
                    std::streamsize fileSize = file.tellg();
                    totalSize += fileSize;

                    // Extract the file extension
                    std::string fileName(fileData.cFileName);
                    size_t extensionPos = fileName.rfind('.');
                    if (extensionPos != std::string::npos) {
                        std::string extension = fileName.substr(extensionPos + 1);

                        // Update the file types map
                        if (fileTypes.find(extension) != fileTypes.end()) {
                            fileTypes[extension].first++;            // Increment count
                            fileTypes[extension].second += fileSize; // Add to size
                        }
                        else {
                            fileTypes[extension] = std::make_pair(1, fileSize);
                        }
                    }

                    // Track largest files
                    largestFiles.push_back({ fileData.cFileName, fileSize });

                    file.close();
                }
            }
        } while (FindNextFileA(hFind, &fileData));

        FindClose(hFind);
    }
}

std::string formatSize(long long size) {
    const std::string units[] = { "B", "KB", "MB", "GB", "TB" };
    int index = 0;
    while (size >= 1024 && index < sizeof(units) / sizeof(units[0]) - 1) {
        size /= 1024;
        index++;
    }
    return std::to_string(size) + " " + units[index];
}

bool compareFileSize(const FileInfo& fileInfo1, const FileInfo& fileInfo2) {
    return fileInfo1.size > fileInfo2.size;
}

void displayLargestFiles(const std::vector<FileInfo>& largestFiles, int count) {
    std::cout << "Largest Files:" << std::endl;
    for (int i = 0; i < count && i < largestFiles.size(); ++i) {
        std::cout << "  " << largestFiles[i].name << " (" << formatSize(largestFiles[i].size) << ")" << std::endl;
    }
}

void displayFileTypes(const std::map<std::string, std::pair<int, long long>>& fileTypes) {
    std::cout << "File Type Distribution:" << std::endl;
    for (const auto& fileType : fileTypes) {
        std::cout << "  ." << fileType.first << ": " << fileType.second.first << " files, Total Size: "
             << formatSize(fileType.second.second) << std::endl;
    }
}

int main() {
    std::string directoryPath = "C:/MinGW/bin";
    long long totalSize = 0;
    std::map<std::string, std::pair<int, long long>> fileTypes;
    std::vector<FileInfo> largestFiles;

    analyzeDirectory(directoryPath, totalSize, fileTypes, largestFiles);

    std::cout << "Total size of directory \"" << directoryPath << "\": " << formatSize(totalSize) << std::endl;

    displayFileTypes(fileTypes);

    std::sort(largestFiles.begin(), largestFiles.end(), compareFileSize);
    displayLargestFiles(largestFiles, 5); // Display the top 5 largest files
     
    return 0;
}
