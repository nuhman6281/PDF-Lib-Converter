/**
 * @file mswin_printer_device.cpp
 * @brief Windows printer device implementation
 * @version 1.0.0
 * @author PDF_LIB Team
 */

#include "mswin_printer_device.h"
#include "../core/pdf_engine.h"
#include "../core/resource_manager.h"
#include "../core/error_handler.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <winspool.h>
#include <tchar.h>
#endif

namespace PDFLib {

MSWinPrinterDevice::MSWinPrinterDevice(std::shared_ptr<PDFEngine> engine, 
                                       std::shared_ptr<ResourceManager> resource_manager)
    : pdf_engine_(engine)
    , resource_manager_(resource_manager)
    , is_open_(false)
    , is_initialized_(false)
    , job_started_(false)
    , page_started_(false)
    , copies_(1)
    , collate_(false)
    , orientation_(0) {
#ifdef _WIN32
    printer_dc_ = nullptr;
    printer_handle_ = nullptr;
#endif
}

MSWinPrinterDevice::~MSWinPrinterDevice() {
    Close();
}

bool MSWinPrinterDevice::Initialize(const ProcessingOptions& options) {
    if (is_initialized_) {
        ErrorHandler::GetInstance().SetError(-1, "Device already initialized");
        return false;
    }
    
    // Store printer settings from options
    copies_ = options.num_copies;
    printer_name_ = options.printer_name;
    
    is_initialized_ = true;
    return true;
}

bool MSWinPrinterDevice::Open(const std::string& printer_name) {
    if (!is_initialized_) {
        ErrorHandler::GetInstance().SetError(-1, "Device not initialized");
        return false;
    }
    
    if (is_open_) {
        Close();
    }
    
    if (!printer_name.empty()) {
        printer_name_ = printer_name;
    }
    
    if (printer_name_.empty()) {
        printer_name_ = GetDefaultPrinter();
        if (printer_name_.empty()) {
            ErrorHandler::GetInstance().SetError(-1, "No default printer found");
            return false;
        }
    }
    
#ifdef _WIN32
    if (!InitializeWindowsPrinter()) {
        return false;
    }
#endif
    
    is_open_ = true;
    return true;
}

bool MSWinPrinterDevice::Close() {
    if (!is_open_) {
        return true;
    }
    
    if (page_started_) {
        EndPage();
    }
    
    if (job_started_) {
        EndJob();
    }
    
#ifdef _WIN32
    CleanupWindowsPrinter();
#endif
    
    is_open_ = false;
    job_started_ = false;
    page_started_ = false;
    return true;
}

bool MSWinPrinterDevice::StartJob(const std::string& job_name) {
    if (!is_open_) {
        ErrorHandler::GetInstance().SetError(-1, "Device not open");
        return false;
    }
    
    if (job_started_) {
        ErrorHandler::GetInstance().SetError(-1, "Job already started");
        return false;
    }
    
#ifdef _WIN32
    if (printer_dc_) {
        DOCINFO di;
        ZeroMemory(&di, sizeof(di));
        di.cbSize = sizeof(di);
        di.lpszDocName = job_name.empty() ? L"PDF_LIB Print Job" : 
                        std::wstring(job_name.begin(), job_name.end()).c_str();
        di.lpszOutput = nullptr;
        di.lpszDatatype = nullptr;
        di.fwType = 0;
        
        if (StartDoc(printer_dc_, &di) <= 0) {
            ErrorHandler::GetInstance().SetError(-1, "Failed to start print job");
            return false;
        }
    }
#endif
    
    job_started_ = true;
    return true;
}

bool MSWinPrinterDevice::EndJob() {
    if (!job_started_) {
        return true;
    }
    
#ifdef _WIN32
    if (printer_dc_) {
        if (EndDoc(printer_dc_) <= 0) {
            ErrorHandler::GetInstance().SetError(-1, "Failed to end print job");
            return false;
        }
    }
#endif
    
    job_started_ = false;
    return true;
}

bool MSWinPrinterDevice::StartPage() {
    if (!job_started_) {
        ErrorHandler::GetInstance().SetError(-1, "No job started");
        return false;
    }
    
    if (page_started_) {
        ErrorHandler::GetInstance().SetError(-1, "Page already started");
        return false;
    }
    
#ifdef _WIN32
    if (printer_dc_) {
        if (StartPage(printer_dc_) <= 0) {
            ErrorHandler::GetInstance().SetError(-1, "Failed to start page");
            return false;
        }
    }
#endif
    
    page_started_ = true;
    return true;
}

bool MSWinPrinterDevice::EndPage() {
    if (!page_started_) {
        return true;
    }
    
#ifdef _WIN32
    if (printer_dc_) {
        if (EndPage(printer_dc_) <= 0) {
            ErrorHandler::GetInstance().SetError(-1, "Failed to end page");
            return false;
        }
    }
#endif
    
    page_started_ = false;
    return true;
}

bool MSWinPrinterDevice::PrintPage(const std::vector<uint8_t>& page_data, int page_number) {
    if (!page_started_) {
        if (!StartPage()) {
            return false;
        }
    }
    
    if (page_data.empty()) {
        ErrorHandler::GetInstance().SetError(-1, "Empty page data");
        return false;
    }
    
#ifdef _WIN32
    if (printer_dc_) {
        // This is a simplified implementation
        // In a real implementation, you would need to convert PDF page data
        // to a format that Windows GDI can handle (e.g., EMF, bitmap)
        
        // For now, we'll just write the raw data
        // This would need to be enhanced with proper PDF-to-GDI conversion
        
        // Placeholder for actual printing logic
        std::cout << "Printing page " << page_number << " to " << printer_name_ << std::endl;
    }
#endif
    
    if (!EndPage()) {
        return false;
    }
    
    return true;
}

void MSWinPrinterDevice::SetPrinterSettings(int copies, bool collate, int orientation) {
    copies_ = copies;
    collate_ = collate;
    orientation_ = orientation;
}

std::vector<std::string> MSWinPrinterDevice::GetAvailablePrinters() {
    std::vector<std::string> printers;
    
#ifdef _WIN32
    DWORD flags = PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS;
    DWORD needed = 0;
    DWORD returned = 0;
    
    // Get required buffer size
    EnumPrinters(flags, nullptr, 1, nullptr, 0, &needed, &returned);
    
    if (needed > 0) {
        std::vector<uint8_t> buffer(needed);
        PRINTER_INFO_1* printer_info = reinterpret_cast<PRINTER_INFO_1*>(buffer.data());
        
        if (EnumPrinters(flags, nullptr, 1, buffer.data(), needed, &needed, &returned)) {
            for (DWORD i = 0; i < returned; ++i) {
                if (printer_info[i].pName) {
                    std::wstring wname(printer_info[i].pName);
                    printers.push_back(std::string(wname.begin(), wname.end()));
                }
            }
        }
    }
#endif
    
    return printers;
}

std::string MSWinPrinterDevice::GetDefaultPrinter() {
    std::string default_printer;
    
#ifdef _WIN32
    DWORD needed = 0;
    GetDefaultPrinter(nullptr, &needed);
    
    if (needed > 0) {
        std::vector<TCHAR> buffer(needed);
        if (GetDefaultPrinter(buffer.data(), &needed)) {
            std::wstring wname(buffer.data());
            default_printer = std::string(wname.begin(), wname.end());
        }
    }
#endif
    
    return default_printer;
}

bool MSWinPrinterDevice::IsOpen() const {
    return is_open_;
}

#ifdef _WIN32
bool MSWinPrinterDevice::InitializeWindowsPrinter() {
    std::wstring wprinter_name(printer_name_.begin(), printer_name_.end());
    
    // Open printer
    if (!OpenPrinter(const_cast<LPWSTR>(wprinter_name.c_str()), &printer_handle_, nullptr)) {
        ErrorHandler::GetInstance().SetError(-1, "Failed to open printer: " + printer_name_);
        return false;
    }
    
    // Get printer DC
    printer_dc_ = CreateDC(nullptr, wprinter_name.c_str(), nullptr, nullptr);
    if (!printer_dc_) {
        ErrorHandler::GetInstance().SetError(-1, "Failed to create printer DC");
        ClosePrinter(printer_handle_);
        printer_handle_ = nullptr;
        return false;
    }
    
    return true;
}

void MSWinPrinterDevice::CleanupWindowsPrinter() {
    if (printer_dc_) {
        DeleteDC(printer_dc_);
        printer_dc_ = nullptr;
    }
    
    if (printer_handle_) {
        ClosePrinter(printer_handle_);
        printer_handle_ = nullptr;
    }
}
#else
bool MSWinPrinterDevice::InitializeWindowsPrinter() {
    ErrorHandler::GetInstance().SetError(-1, "Windows printer support not available on this platform");
    return false;
}

void MSWinPrinterDevice::CleanupWindowsPrinter() {
    // No-op on non-Windows platforms
}
#endif

bool MSWinPrinterDevice::Open() {
    return Open("");
}

bool MSWinPrinterDevice::WritePage(const std::vector<uint8_t>& page_data, int page_number) {
    return PrintPage(page_data, page_number);
}

bool MSWinPrinterDevice::Finalize() {
    if (job_started_) {
        return EndJob();
    }
    return true;
}

std::string MSWinPrinterDevice::GetName() const {
    return "MSWinPrinter";
}

bool MSWinPrinterDevice::IsInitialized() const {
    return is_initialized_;
}

std::vector<std::string> MSWinPrinterDevice::GetSupportedExtensions() const {
    return {".ps", ".eps", ".pdf"};
}

std::string MSWinPrinterDevice::GetCapabilities() const {
    return "Windows printer device supporting PostScript and PDF printing";
}

bool MSWinPrinterDevice::SetParameter(const std::string& name, const std::string& value) {
    if (name == "printer_name") {
        printer_name_ = value;
        return true;
    } else if (name == "copies") {
        copies_ = std::stoi(value);
        return true;
    } else if (name == "orientation") {
        orientation_ = std::stoi(value);
        return true;
    }
    return false;
}

std::string MSWinPrinterDevice::GetParameter(const std::string& name) const {
    if (name == "printer_name") {
        return printer_name_;
    } else if (name == "copies") {
        return std::to_string(copies_);
    } else if (name == "orientation") {
        return std::to_string(orientation_);
    }
    return "";
}

} // namespace PDFLib
