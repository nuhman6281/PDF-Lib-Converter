/**
 * @file resource_manager.h
 * @brief Resource manager for PDF_LIB
 */

#ifndef PDF_LIB_RESOURCE_MANAGER_H
#define PDF_LIB_RESOURCE_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include "error_handler.h"

namespace PDFLib {

/**
 * @brief Resource types
 */
enum class ResourceType {
    TEMP_FILE,      ///< Temporary file
    MEMORY_BUFFER,  ///< Memory buffer
    HANDLE,         ///< System handle
    STREAM          ///< Data stream
};

/**
 * @brief Resource information
 */
struct ResourceInfo {
    ResourceType type;
    std::string identifier;    ///< Resource identifier
    std::string path;          ///< File path (for files)
    size_t size;               ///< Resource size
    void* handle;              ///< System handle
    bool auto_cleanup;         ///< Whether to auto-cleanup
};

/**
 * @brief Resource manager class
 * 
 * Manages temporary files, memory buffers, and other system resources
 * with automatic cleanup capabilities.
 */
class ResourceManager {
public:
    /**
     * @brief Constructor
     */
    ResourceManager();
    
    /**
     * @brief Destructor
     */
    ~ResourceManager();
    
    /**
     * @brief Initialize resource manager
     * @param error_handler Error handler reference
     * @return true if initialization successful, false otherwise
     */
    bool Initialize(ErrorHandler& error_handler);
    
    /**
     * @brief Create temporary file
     * @param prefix File prefix
     * @param extension File extension
     * @param auto_cleanup Whether to auto-cleanup on destruction
     * @return Temporary file path or empty string on failure
     */
    std::string CreateTempFile(const std::string& prefix = "pdf_lib_",
                              const std::string& extension = ".tmp",
                              bool auto_cleanup = true);
    
    /**
     * @brief Create memory buffer
     * @param size Buffer size
     * @param auto_cleanup Whether to auto-cleanup on destruction
     * @return Pointer to memory buffer or nullptr on failure
     */
    void* CreateMemoryBuffer(size_t size, bool auto_cleanup = true);
    
    /**
     * @brief Register existing file as managed resource
     * @param filepath File path
     * @param auto_cleanup Whether to auto-cleanup on destruction
     * @return true if registration successful, false otherwise
     */
    bool RegisterFile(const std::string& filepath, bool auto_cleanup = true);
    
    /**
     * @brief Register existing memory buffer as managed resource
     * @param buffer Buffer pointer
     * @param size Buffer size
     * @param auto_cleanup Whether to auto-cleanup on destruction
     * @return true if registration successful, false otherwise
     */
    bool RegisterMemoryBuffer(void* buffer, size_t size, bool auto_cleanup = true);
    
    /**
     * @brief Delete temporary file
     * @param filepath File path to delete
     * @return true if deletion successful, false otherwise
     */
    bool DeleteTempFile(const std::string& filepath);
    
    /**
     * @brief Free memory buffer
     * @param buffer Buffer pointer to free
     * @return true if freeing successful, false otherwise
     */
    bool FreeMemoryBuffer(void* buffer);
    
    /**
     * @brief Get resource information
     * @param identifier Resource identifier
     * @return Resource information or empty info if not found
     */
    ResourceInfo GetResourceInfo(const std::string& identifier) const;
    
    /**
     * @brief Get all managed resources
     * @return Vector of resource information
     */
    std::vector<ResourceInfo> GetAllResources() const;
    
    /**
     * @brief Check if resource exists
     * @param identifier Resource identifier
     * @return true if resource exists, false otherwise
     */
    bool ResourceExists(const std::string& identifier) const;
    
    /**
     * @brief Get total memory usage
     * @return Total memory usage in bytes
     */
    size_t GetTotalMemoryUsage() const;
    
    /**
     * @brief Get total disk usage
     * @return Total disk usage in bytes
     */
    size_t GetTotalDiskUsage() const;
    
    /**
     * @brief Clean up specific resource
     * @param identifier Resource identifier
     * @return true if cleanup successful, false otherwise
     */
    bool CleanupResource(const std::string& identifier);
    
    /**
     * @brief Clean up all resources
     * @param force Force cleanup even if auto_cleanup is false
     */
    void CleanupAllResources(bool force = false);
    
    /**
     * @brief Set temporary directory
     * @param temp_dir Temporary directory path
     * @return true if setting successful, false otherwise
     */
    bool SetTempDirectory(const std::string& temp_dir);
    
    /**
     * @brief Get temporary directory
     * @return Temporary directory path
     */
    std::string GetTempDirectory() const;
    
    /**
     * @brief Clean up resources
     */
    void Cleanup();

private:
    /**
     * @brief Generate unique identifier
     * @param prefix Identifier prefix
     * @return Unique identifier
     */
    std::string GenerateUniqueId(const std::string& prefix = "res_") const;
    
    /**
     * @brief Get system temporary directory
     * @return System temporary directory path
     */
    std::string GetSystemTempDirectory() const;
    
    /**
     * @brief Create directory if it doesn't exist
     * @param dir_path Directory path
     * @return true if creation successful, false otherwise
     */
    bool CreateDirectoryIfNotExists(const std::string& dir_path) const;
    
    /**
     * @brief Check if file exists
     * @param filepath File path to check
     * @return true if file exists, false otherwise
     */
    bool FileExists(const std::string& filepath) const;
    
    /**
     * @brief Get file size
     * @param filepath File path
     * @return File size in bytes or 0 on failure
     */
    size_t GetFileSize(const std::string& filepath) const;

    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace PDFLib

#endif // PDF_LIB_RESOURCE_MANAGER_H 