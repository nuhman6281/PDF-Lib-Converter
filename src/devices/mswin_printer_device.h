/**
 * @file mswin_printer_device.h
 * @brief Windows printer device for printing operations
 * @version 1.0.0
 * @author PDF_LIB Team
 */

#ifndef MSWIN_PRINTER_DEVICE_H
#define MSWIN_PRINTER_DEVICE_H

#include <string>
#include <memory>
#include <vector>
#include "../core/device.h"
#include "../core/device_manager.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace PDFLib {

class PDFEngine;
class ResourceManager;

/**
 * @brief Windows printer device for printing operations
 */
class MSWinPrinterDevice : public Device {
public:
    /**
     * @brief Constructor
     * @param engine PDF engine instance
     * @param resource_manager Resource manager instance
     */
    MSWinPrinterDevice(std::shared_ptr<PDFEngine> engine, 
                       std::shared_ptr<ResourceManager> resource_manager);
    
    /**
     * @brief Destructor
     */
    ~MSWinPrinterDevice() override;
    
    /**
     * @brief Initialize the device
     * @param options Processing options
     * @return true if initialization successful, false otherwise
     */
    bool Initialize(const ProcessingOptions& options) override;
    
    /**
     * @brief Open the printer device (Device interface)
     * @return true if opened successfully, false otherwise
     */
    bool Open() override;
    
    /**
     * @brief Open the printer device
     * @param printer_name Printer name (empty for default)
     * @return true if opened successfully, false otherwise
     */
    bool Open(const std::string& printer_name);
    
    /**
     * @brief Close the device
     */
    bool Close() override;
    
    /**
     * @brief Start a print job
     * @param job_name Job name
     * @return true if started successfully, false otherwise
     */
    bool StartJob(const std::string& job_name);
    
    /**
     * @brief End the current print job
     * @return true if ended successfully, false otherwise
     */
    bool EndJob();
    
    /**
     * @brief Start a new page
     * @return true if started successfully, false otherwise
     */
    bool StartPage();
    
    /**
     * @brief End the current page
     * @return true if ended successfully, false otherwise
     */
    bool EndPage();
    
    /**
     * @brief Print a page
     * @param page_data Page data
     * @param page_number Page number
     * @return true if print successful, false otherwise
     */
    bool PrintPage(const std::vector<uint8_t>& page_data, int page_number);
    
    /**
     * @brief Set printer settings
     * @param copies Number of copies
     * @param collate Collate copies
     * @param orientation Page orientation (0=portrait, 1=landscape)
     */
    void SetPrinterSettings(int copies, bool collate, int orientation);
    
    /**
     * @brief Get available printers
     * @return Vector of printer names
     */
    static std::vector<std::string> GetAvailablePrinters();
    
    /**
     * @brief Get default printer name
     * @return Default printer name
     */
    static std::string GetDefaultPrinter();
    
    /**
     * @brief Get device type
     * @return Device type
     */
    DeviceType GetType() const override { return DeviceType::MSWIN_PRINTER; }
    
    /**
     * @brief Check if device is open
     * @return true if open, false otherwise
     */
    bool IsOpen() const override;
    
    /**
     * @brief Write page data to the device
     * @param page_data Page data to write
     * @param page_number Page number (1-based)
     * @return true if successful, false otherwise
     */
    bool WritePage(const std::vector<uint8_t>& page_data, int page_number) override;
    
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
    std::string printer_name_;
    bool is_open_;
    bool is_initialized_;
    bool job_started_;
    bool page_started_;
    
    // Printer settings
    int copies_;
    bool collate_;
    int orientation_;
    
#ifdef _WIN32
    HDC printer_dc_;
    HANDLE printer_handle_;
#endif
    
    /**
     * @brief Initialize Windows printer APIs
     * @return true if successful, false otherwise
     */
    bool InitializeWindowsPrinter();
    
    /**
     * @brief Clean up Windows printer resources
     */
    void CleanupWindowsPrinter();
};

} // namespace PDFLib

#endif // MSWIN_PRINTER_DEVICE_H 