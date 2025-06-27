/**
 * @file file_utils.h
 * @brief File utility functions for PDF_LIB
 */

#ifndef PDF_LIB_FILE_UTILS_H
#define PDF_LIB_FILE_UTILS_H

#include <string>
#include <vector>

namespace PDFLib {
namespace Utils {

/**
 * @brief Check if file exists
 * @param filepath File path to check
 * @return true if file exists, false otherwise
 */
bool FileExists(const std::string& filepath);

/**
 * @brief Get file extension
 * @param filepath File path
 * @return File extension (lowercase)
 */
std::string GetFileExtension(const std::string& filepath);

/**
 * @brief Create temporary file
 * @param prefix File prefix
 * @param extension File extension
 * @return Temporary file path
 */
std::string CreateTempFile(const std::string& prefix, 
                          const std::string& extension);

/**
 * @brief Delete file
 * @param filepath File path to delete
 * @return true if deletion successful, false otherwise
 */
bool DeleteFile(const std::string& filepath);

/**
 * @brief Get temporary directory
 * @return Temporary directory path
 */
std::string GetTempDirectory();

/**
 * @brief Get current directory
 * @return Current directory path
 */
std::string GetCurrentDirectory();

/**
 * @brief Create directory
 * @param dirpath Directory path
 * @return true if creation successful, false otherwise
 */
bool CreateDirectory(const std::string& dirpath);

/**
 * @brief Create directory recursively
 * @param dirpath Directory path
 * @return true if creation successful, false otherwise
 */
bool CreateDirectoryRecursive(const std::string& dirpath);

/**
 * @brief Check if directory exists
 * @param dirpath Directory path
 * @return true if directory exists, false otherwise
 */
bool DirectoryExists(const std::string& dirpath);

/**
 * @brief Get file size
 * @param filepath File path
 * @return File size in bytes or 0 on failure
 */
size_t GetFileSize(const std::string& filepath);

/**
 * @brief Get file name from path
 * @param filepath File path
 * @return File name with extension
 */
std::string GetFileName(const std::string& filepath);

/**
 * @brief Get directory name from path
 * @param filepath File path
 * @return Directory name
 */
std::string GetDirectoryName(const std::string& filepath);

/**
 * @brief Combine two paths
 * @param path1 First path
 * @param path2 Second path
 * @return Combined path
 */
std::string CombinePath(const std::string& path1, const std::string& path2);

/**
 * @brief Check if path is absolute
 * @param filepath File path
 * @return true if path is absolute, false otherwise
 */
bool IsAbsolutePath(const std::string& filepath);

/**
 * @brief Get absolute path
 * @param filepath File path
 * @return Absolute path
 */
std::string GetAbsolutePath(const std::string& filepath);

/**
 * @brief Generate unique identifier
 * @return Unique identifier string
 */
std::string GenerateUniqueId();

} // namespace Utils
} // namespace PDFLib

#endif // PDF_LIB_FILE_UTILS_H 