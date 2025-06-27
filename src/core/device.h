/**
 * @file device.h
 * @brief Base device class for PDF_LIB
 */

#ifndef PDF_LIB_DEVICE_H
#define PDF_LIB_DEVICE_H

#include <string>
#include <memory>
#include <vector>
#include "../../include/pdf_lib.h"

namespace PDFLib {

/**
 * @brief Base class for all output devices
 * 
 * This abstract class defines the interface that all device implementations
 * must follow. Devices handle the actual output of processed data to various
 * destinations like files, printers, or memory buffers.
 */
class Device {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~Device() = default;
    
    /**
     * @brief Initialize the device
     * @param options Processing options
     * @return true if successful, false otherwise
     */
    virtual bool Initialize(const ProcessingOptions& options) = 0;
    
    /**
     * @brief Open the device for output
     * @return true if successful, false otherwise
     */
    virtual bool Open() = 0;
    
    /**
     * @brief Close the device
     * @return true if successful, false otherwise
     */
    virtual bool Close() = 0;
    
    /**
     * @brief Write page data to the device
     * @param page_data Page data to write
     * @param page_number Page number (1-based)
     * @return true if successful, false otherwise
     */
    virtual bool WritePage(const std::vector<uint8_t>& page_data, int page_number) = 0;
    
    /**
     * @brief Finalize the output
     * @return true if successful, false otherwise
     */
    virtual bool Finalize() = 0;
    
    /**
     * @brief Get device type
     * @return Device type
     */
    virtual DeviceType GetType() const = 0;
    
    /**
     * @brief Get device name
     * @return Device name
     */
    virtual std::string GetName() const = 0;
    
    /**
     * @brief Check if device is initialized
     * @return true if initialized, false otherwise
     */
    virtual bool IsInitialized() const = 0;
    
    /**
     * @brief Check if device is open
     * @return true if open, false otherwise
     */
    virtual bool IsOpen() const = 0;
    
    /**
     * @brief Get supported file extensions
     * @return Vector of supported extensions (e.g., {".pdf", ".eps"})
     */
    virtual std::vector<std::string> GetSupportedExtensions() const = 0;
    
    /**
     * @brief Get device capabilities
     * @return Device capabilities as string
     */
    virtual std::string GetCapabilities() const = 0;
    
    /**
     * @brief Set device-specific parameter
     * @param name Parameter name
     * @param value Parameter value
     * @return true if successful, false otherwise
     */
    virtual bool SetParameter(const std::string& name, const std::string& value) = 0;
    
    /**
     * @brief Get device-specific parameter
     * @param name Parameter name
     * @return Parameter value, empty string if not found
     */
    virtual std::string GetParameter(const std::string& name) const = 0;

protected:
    /**
     * @brief Protected constructor to prevent direct instantiation
     */
    Device() = default;
    
    /**
     * @brief Copy constructor (deleted)
     */
    Device(const Device&) = delete;
    
    /**
     * @brief Assignment operator (deleted)
     */
    Device& operator=(const Device&) = delete;
};

} // namespace PDFLib

#endif // PDF_LIB_DEVICE_H 