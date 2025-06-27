/**
 * @file file_utils.cpp
 * @brief File utility functions implementation
 */

#include "file_utils.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

#ifdef _WIN32
#include <windows.h>
#include <shlwapi.h>
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <cstring>
#endif

namespace PDFLib {
namespace Utils {

bool FileExists(const std::string& filepath) {
    if (filepath.empty()) {
        return false;
    }
    
#ifdef _WIN32
    DWORD attributes = GetFileAttributesA(filepath.c_str());
    return (attributes != INVALID_FILE_ATTRIBUTES && 
            !(attributes & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat buffer;
    return (stat(filepath.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
#endif
}

std::string GetFileExtension(const std::string& filepath) {
    if (filepath.empty()) {
        return "";
    }
    
    size_t last_dot = filepath.find_last_of('.');
    if (last_dot == std::string::npos) {
        return "";
    }
    
    std::string extension = filepath.substr(last_dot + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return extension;
}

std::string CreateTempFile(const std::string& prefix, const std::string& extension) {
    std::string temp_dir = GetTempDirectory();
    if (temp_dir.empty()) {
        return "";
    }
    
    // Ensure temp directory ends with separator
    if (temp_dir.back() != '/' && temp_dir.back() != '\\') {
#ifdef _WIN32
        temp_dir += '\\';
#else
        temp_dir += '/';
#endif
    }
    
    // Generate unique filename
    std::string filename = prefix + GenerateUniqueId() + extension;
    std::string full_path = temp_dir + filename;
    
    // Create empty file
    std::ofstream file(full_path);
    if (!file.is_open()) {
        return "";
    }
    file.close();
    
    return full_path;
}

bool DeleteFile(const std::string& filepath) {
    if (filepath.empty()) {
        return false;
    }
    
#ifdef _WIN32
    return DeleteFileA(filepath.c_str()) != 0;
#else
    return unlink(filepath.c_str()) == 0;
#endif
}

std::string GetTempDirectory() {
#ifdef _WIN32
    char temp_path[MAX_PATH];
    DWORD result = GetTempPathA(MAX_PATH, temp_path);
    if (result == 0 || result > MAX_PATH) {
        return "";
    }
    return std::string(temp_path);
#else
    const char* temp_dir = getenv("TMPDIR");
    if (temp_dir) {
        return std::string(temp_dir);
    }
    
    temp_dir = getenv("TMP");
    if (temp_dir) {
        return std::string(temp_dir);
    }
    
    temp_dir = getenv("TEMP");
    if (temp_dir) {
        return std::string(temp_dir);
    }
    
    return "/tmp";
#endif
}

std::string GetCurrentDirectory() {
#ifdef _WIN32
    char current_dir[MAX_PATH];
    DWORD result = GetCurrentDirectoryA(MAX_PATH, current_dir);
    if (result == 0 || result > MAX_PATH) {
        return "";
    }
    return std::string(current_dir);
#else
    char current_dir[PATH_MAX];
    if (getcwd(current_dir, PATH_MAX) == nullptr) {
        return "";
    }
    return std::string(current_dir);
#endif
}

bool CreateDirectory(const std::string& dirpath) {
    if (dirpath.empty()) {
        return false;
    }
    
#ifdef _WIN32
    return CreateDirectoryA(dirpath.c_str(), nullptr) != 0;
#else
    return mkdir(dirpath.c_str(), 0755) == 0;
#endif
}

bool CreateDirectoryRecursive(const std::string& dirpath) {
    if (dirpath.empty()) {
        return false;
    }
    
    // Split path into components
    std::vector<std::string> components;
    std::string current_path;
    
#ifdef _WIN32
    std::string separator = "\\";
    std::string path = dirpath;
    std::replace(path.begin(), path.end(), '/', '\\');
#else
    std::string separator = "/";
    std::string path = dirpath;
#endif
    
    size_t pos = 0;
    while ((pos = path.find(separator)) != std::string::npos) {
        components.push_back(path.substr(0, pos));
        path.erase(0, pos + separator.length());
    }
    if (!path.empty()) {
        components.push_back(path);
    }
    
    // Create each component
    std::string full_path;
    for (const auto& component : components) {
        if (component.empty()) {
            continue;
        }
        
        if (full_path.empty()) {
            full_path = component;
        } else {
            full_path += separator + component;
        }
        
        if (!DirectoryExists(full_path)) {
            if (!CreateDirectory(full_path)) {
                return false;
            }
        }
    }
    
    return true;
}

bool DirectoryExists(const std::string& dirpath) {
    if (dirpath.empty()) {
        return false;
    }
    
#ifdef _WIN32
    DWORD attributes = GetFileAttributesA(dirpath.c_str());
    return (attributes != INVALID_FILE_ATTRIBUTES && 
            (attributes & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat buffer;
    return (stat(dirpath.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
#endif
}

size_t GetFileSize(const std::string& filepath) {
    if (!FileExists(filepath)) {
        return 0;
    }
    
#ifdef _WIN32
    WIN32_FILE_ATTRIBUTE_DATA file_data;
    if (!GetFileAttributesExA(filepath.c_str(), GetFileExInfoStandard, &file_data)) {
        return 0;
    }
    
    ULARGE_INTEGER file_size;
    file_size.LowPart = file_data.nFileSizeLow;
    file_size.HighPart = file_data.nFileSizeHigh;
    return static_cast<size_t>(file_size.QuadPart);
#else
    struct stat buffer;
    if (stat(filepath.c_str(), &buffer) != 0) {
        return 0;
    }
    return static_cast<size_t>(buffer.st_size);
#endif
}

std::string GetFileName(const std::string& filepath) {
    if (filepath.empty()) {
        return "";
    }
    
#ifdef _WIN32
    char drive[MAX_PATH];
    char dir[MAX_PATH];
    char filename[MAX_PATH];
    char ext[MAX_PATH];
    
    if (_splitpath_s(filepath.c_str(), drive, MAX_PATH, dir, MAX_PATH, 
                     filename, MAX_PATH, ext, MAX_PATH) != 0) {
        return "";
    }
    
    return std::string(filename) + std::string(ext);
#else
    size_t last_slash = filepath.find_last_of('/');
    if (last_slash == std::string::npos) {
        return filepath;
    }
    return filepath.substr(last_slash + 1);
#endif
}

std::string GetDirectoryName(const std::string& filepath) {
    if (filepath.empty()) {
        return "";
    }
    
#ifdef _WIN32
    char drive[MAX_PATH];
    char dir[MAX_PATH];
    char filename[MAX_PATH];
    char ext[MAX_PATH];
    
    if (_splitpath_s(filepath.c_str(), drive, MAX_PATH, dir, MAX_PATH, 
                     filename, MAX_PATH, ext, MAX_PATH) != 0) {
        return "";
    }
    
    return std::string(drive) + std::string(dir);
#else
    size_t last_slash = filepath.find_last_of('/');
    if (last_slash == std::string::npos) {
        return "";
    }
    return filepath.substr(0, last_slash);
#endif
}

std::string CombinePath(const std::string& path1, const std::string& path2) {
    if (path1.empty()) {
        return path2;
    }
    if (path2.empty()) {
        return path1;
    }
    
#ifdef _WIN32
    char combined[MAX_PATH];
    if (PathCombineA(combined, path1.c_str(), path2.c_str()) == nullptr) {
        return "";
    }
    return std::string(combined);
#else
    std::string result = path1;
    if (result.back() != '/') {
        result += '/';
    }
    if (path2.front() == '/') {
        result += path2.substr(1);
    } else {
        result += path2;
    }
    return result;
#endif
}

bool IsAbsolutePath(const std::string& filepath) {
    if (filepath.empty()) {
        return false;
    }
    
#ifdef _WIN32
    return PathIsRelativeA(filepath.c_str()) == FALSE;
#else
    return filepath.front() == '/';
#endif
}

std::string GetAbsolutePath(const std::string& filepath) {
    if (filepath.empty()) {
        return "";
    }
    
    if (IsAbsolutePath(filepath)) {
        return filepath;
    }
    
    std::string current_dir = GetCurrentDirectory();
    if (current_dir.empty()) {
        return filepath;
    }
    
    return CombinePath(current_dir, filepath);
}

std::string GenerateUniqueId() {
    static int counter = 0;
    std::stringstream ss;
    ss << std::time(nullptr) << "_" << counter++;
    return ss.str();
}

} // namespace Utils
} // namespace PDFLib 