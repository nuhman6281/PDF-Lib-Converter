/**
 * @file pdf_write_device.h
 * @brief PDF write device for outputting PDF files
 * @version 1.0.0
 * @author PDF_LIB Team
 */

#ifndef PDF_WRITE_DEVICE_H
#define PDF_WRITE_DEVICE_H

#include <string>
#include <memory>
#include <vector>
#include "../core/device.h"
#include "../core/device_manager.h"

namespace PDFLib {

class PDFEngine;
class ResourceManager;

/**
 * @brief PDF write device for creating and writing PDF files
 */
class PDFWriteDevice : public Device {
public:
    /**
     * @brief Constructor
     * @param engine PDF engine instance
     * @param resource_manager Resource manager instance
     */
    PDFWriteDevice(std::shared_ptr<PDFEngine> engine, 
                   std::shared_ptr<ResourceManager> resource_manager);
    
    /**
     * @brief Destructor
     */
    ~PDFWriteDevice() override;
    
    /**
     * @brief Initialize the device
     * @param options Processing options
     * @return true if initialization successful, false otherwise
     */
    bool Initialize(const ProcessingOptions& options) override;
    
    /**
     * @brief Open the device for writing
     * @return true if opened successfully, false otherwise
     */
    bool Open() override;
    
    /**
     * @brief Open the device for writing with output file
     * @param output_file Output file path
     * @return true if opened successfully, false otherwise
     */
    bool Open(const std::string& output_file);
    
    /**
     * @brief Close the device
     */
    bool Close() override;
    
    /**
     * @brief Write a page to the PDF
     * @param page_data Page data
     * @param page_number Page number
     * @return true if write successful, false otherwise
     */
    bool WritePage(const std::vector<uint8_t>& page_data, int page_number) override;
    
    /**
     * @brief Set PDF metadata
     * @param title Document title
     * @param author Document author
     * @param subject Document subject
     * @param keywords Document keywords
     */
    void SetMetadata(const std::string& title, const std::string& author,
                     const std::string& subject, const std::string& keywords);
    
    /**
     * @brief Set PDF security options
     * @param user_password User password
     * @param owner_password Owner password
     * @param permissions Security permissions
     */
    void SetSecurity(const std::string& user_password, const std::string& owner_password,
                     int permissions);
    
    /**
     * @brief Get device type
     * @return Device type
     */
    DeviceType GetType() const override { return DeviceType::PDF_WRITE; }
    
    /**
     * @brief Check if device is open
     * @return true if open, false otherwise
     */
    bool IsOpen() const override;
    
    /**
     * @brief Finalize the output
     * @return true if successful, false otherwise
     */
    bool Finalize() override;
    
    /**
     * @brief Get device name
     * @return Device name
     */
    std::string GetName() const override;
    
    /**
     * @brief Check if device is initialized
     * @return true if initialized, false otherwise
     */
    bool IsInitialized() const override;
    
    /**
     * @brief Get supported file extensions
     * @return Vector of supported extensions
     */
    std::vector<std::string> GetSupportedExtensions() const override;
    
    /**
     * @brief Get device capabilities
     * @return Device capabilities as string
     */
    std::string GetCapabilities() const override;
    
    /**
     * @brief Set device-specific parameter
     * @param name Parameter name
     * @param value Parameter value
     * @return true if successful, false otherwise
     */
    bool SetParameter(const std::string& name, const std::string& value) override;
    
    /**
     * @brief Get device-specific parameter
     * @param name Parameter name
     * @return Parameter value, empty string if not found
     */
    std::string GetParameter(const std::string& name) const override;

private:
    std::shared_ptr<PDFEngine> pdf_engine_;
    std::shared_ptr<ResourceManager> resource_manager_;
    std::string output_file_;
    bool is_open_;
    bool is_initialized_;
    
    // PDF metadata
    std::string title_;
    std::string author_;
    std::string subject_;
    std::string keywords_;
    
    // Security settings
    std::string user_password_;
    std::string owner_password_;
    int permissions_;
    
    // Page tracking
    std::vector<std::vector<uint8_t>> pages_;
    int current_page_;
    
    /**
     * @brief Write the PDF file to disk
     */
    void WritePDFFile();
};

} // namespace PDFLib

#endif // PDF_WRITE_DEVICE_H 