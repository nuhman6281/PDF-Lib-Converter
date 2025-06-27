/**
 * @file device_manager.h
 * @brief Device manager for PDF_LIB
 */

#ifndef PDF_LIB_DEVICE_MANAGER_H
#define PDF_LIB_DEVICE_MANAGER_H

#include <memory>
#include <string>
#include "../include/pdf_lib.h"
#include "error_handler.h"
#include "device.h"

namespace PDFLib {

// Forward declarations
class PDFEngine;
class Device;

/**
 * @brief Device manager class
 * 
 * Manages different output devices (PDF writer, Windows printer, etc.)
 * and coordinates their operations.
 */
class DeviceManager {
public:
    /**
     * @brief Constructor
     */
    DeviceManager();
    
    /**
     * @brief Destructor
     */
    ~DeviceManager();
    
    /**
     * @brief Initialize device manager
     * @param options Processing options
     * @param error_handler Error handler reference
     * @return true if initialization successful, false otherwise
     */
    bool Initialize(const ProcessingOptions& options, ErrorHandler& error_handler);
    
    /**
     * @brief Process PDF using the configured device
     * @param pdf_engine PDF engine reference
     * @param error_handler Error handler reference
     * @return true if processing successful, false otherwise
     */
    bool Process(PDFEngine& pdf_engine, ErrorHandler& error_handler);
    
    /**
     * @brief Get current device
     * @return Pointer to current device or nullptr if none
     */
    std::shared_ptr<Device> GetCurrentDevice() const;
    
    /**
     * @brief Set device by type
     * @param device_type Device type
     * @param options Processing options
     * @param error_handler Error handler reference
     * @return true if device set successfully, false otherwise
     */
    bool SetDevice(DeviceType device_type, const ProcessingOptions& options, 
                   ErrorHandler& error_handler);
    
    /**
     * @brief Clean up resources
     */
    void Cleanup();

private:
    /**
     * @brief Create device by type
     * @param device_type Device type
     * @param options Processing options
     * @param error_handler Error handler reference
     * @return Shared pointer to device or nullptr on failure
     */
    std::shared_ptr<Device> CreateDevice(DeviceType device_type, 
                                        const ProcessingOptions& options,
                                        ErrorHandler& error_handler);

    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace PDFLib

#endif // PDF_LIB_DEVICE_MANAGER_H 