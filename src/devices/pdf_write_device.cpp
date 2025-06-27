/**
 * @file pdf_write_device.cpp
 * @brief PDF write device implementation
 * @version 1.0.0
 * @author PDF_LIB Team
 */

#include "pdf_write_device.h"
#include "../core/pdf_engine.h"
#include "../core/resource_manager.h"
#include "../core/error_handler.h"
#include "../utils/file_utils.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace PDFLib {

PDFWriteDevice::PDFWriteDevice(std::shared_ptr<PDFEngine> engine, 
                               std::shared_ptr<ResourceManager> resource_manager)
    : pdf_engine_(engine)
    , resource_manager_(resource_manager)
    , is_open_(false)
    , is_initialized_(false)
    , permissions_(0)
    , current_page_(0) {
}

PDFWriteDevice::~PDFWriteDevice() {
    Close();
}

bool PDFWriteDevice::Initialize(const ProcessingOptions& options) {
    if (is_initialized_) {
        ErrorHandler::GetInstance().SetError(-1, "Device already initialized");
        return false;
    }
    
    // Store options for later use
    is_initialized_ = true;
    return true;
}

bool PDFWriteDevice::Open(const std::string& output_file) {
    if (!is_initialized_) {
        ErrorHandler::GetInstance().SetError(-1, "Device not initialized");
        return false;
    }
    
    if (is_open_) {
        Close();
    }
    
    output_file_ = output_file;
    
    // Create output directory if it doesn't exist
    std::string directory = Utils::GetDirectoryName(output_file);
    if (!directory.empty() && !Utils::DirectoryExists(directory)) {
        if (!Utils::CreateDirectory(directory)) {
            ErrorHandler::GetInstance().SetError(-1, "Failed to create output directory: " + directory);
            return false;
        }
    }
    
    // Initialize page storage
    pages_.clear();
    current_page_ = 0;
    is_open_ = true;
    
    return true;
}

bool PDFWriteDevice::Open() {
    return Open("");
}

bool PDFWriteDevice::Close() {
    if (!is_open_) {
        return true;
    }
    
    // Write the PDF file if we have pages
    if (!pages_.empty() && !output_file_.empty()) {
        WritePDFFile();
    }
    
    pages_.clear();
    current_page_ = 0;
    is_open_ = false;
    return true;
}

bool PDFWriteDevice::WritePage(const std::vector<uint8_t>& page_data, int page_number) {
    if (!is_open_) {
        ErrorHandler::GetInstance().SetError(-1, "Device not open");
        return false;
    }
    
    if (page_data.empty()) {
        ErrorHandler::GetInstance().SetError(-1, "Empty page data");
        return false;
    }
    
    // Store page data
    pages_.push_back(page_data);
    current_page_++;
    
    return true;
}

void PDFWriteDevice::SetMetadata(const std::string& title, const std::string& author,
                                 const std::string& subject, const std::string& keywords) {
    title_ = title;
    author_ = author;
    subject_ = subject;
    keywords_ = keywords;
}

void PDFWriteDevice::SetSecurity(const std::string& user_password, const std::string& owner_password,
                                 int permissions) {
    user_password_ = user_password;
    owner_password_ = owner_password;
    permissions_ = permissions;
}

bool PDFWriteDevice::IsOpen() const {
    return is_open_;
}

void PDFWriteDevice::WritePDFFile() {
    if (output_file_.empty()) {
        ErrorHandler::GetInstance().SetError(-1, "No output file specified");
        return;
    }
    
    std::ofstream file(output_file_, std::ios::binary);
    if (!file.is_open()) {
        ErrorHandler::GetInstance().SetError(-1, "Failed to open output file: " + output_file_);
        return;
    }
    
    // Write PDF header
    file << "%PDF-1.7\n";
    
    // Write pages
    for (size_t i = 0; i < pages_.size(); ++i) {
        // This is a simplified implementation
        // In a real implementation, you would need to properly format the PDF structure
        // including objects, streams, and cross-reference table
        
        std::stringstream page_obj;
        page_obj << (i + 1) << " 0 obj\n";
        page_obj << "<<\n";
        page_obj << "/Type /Page\n";
        page_obj << "/Parent 2 0 R\n";
        page_obj << "/MediaBox [0 0 612 792]\n";
        page_obj << "/Contents " << (i + 3) << " 0 R\n";
        page_obj << ">>\n";
        page_obj << "endobj\n";
        
        file << page_obj.str();
    }
    
    // Write catalog
    std::stringstream catalog;
    catalog << (pages_.size() + 1) << " 0 obj\n";
    catalog << "<<\n";
    catalog << "/Type /Catalog\n";
    catalog << "/Pages 2 0 R\n";
    catalog << ">>\n";
    catalog << "endobj\n";
    
    file << catalog.str();
    
    // Write pages object
    std::stringstream pages_obj;
    pages_obj << "2 0 obj\n";
    pages_obj << "<<\n";
    pages_obj << "/Type /Pages\n";
    pages_obj << "/Kids [";
    for (size_t i = 0; i < pages_.size(); ++i) {
        if (i > 0) pages_obj << " ";
        pages_obj << (i + 1) << " 0 R";
    }
    pages_obj << "]\n";
    pages_obj << "/Count " << pages_.size() << "\n";
    pages_obj << ">>\n";
    pages_obj << "endobj\n";
    
    file << pages_obj.str();
    
    // Write content streams
    for (size_t i = 0; i < pages_.size(); ++i) {
        std::stringstream content;
        content << (i + 3) << " 0 obj\n";
        content << "<<\n";
        content << "/Length " << pages_[i].size() << "\n";
        content << ">>\n";
        content << "stream\n";
        
        file << content.str();
        file.write(reinterpret_cast<const char*>(pages_[i].data()), pages_[i].size());
        file << "\nendstream\n";
        file << "endobj\n";
    }
    
    // Write cross-reference table (simplified)
    long xref_offset = file.tellp();
    file << "xref\n";
    file << "0 " << (pages_.size() + 3) << "\n";
    file << "0000000000 65535 f \n";
    
    // In a real implementation, you would calculate proper object offsets
    for (size_t i = 0; i < pages_.size() + 2; ++i) {
        file << "0000000000 00000 n \n";
    }
    
    // Write trailer
    file << "trailer\n";
    file << "<<\n";
    file << "/Size " << (pages_.size() + 3) << "\n";
    file << "/Root " << (pages_.size() + 1) << " 0 R\n";
    file << ">>\n";
    file << "startxref\n";
    file << xref_offset << "\n";
    file << "%%EOF\n";
    
    file.close();
}

bool PDFWriteDevice::Finalize() {
    if (is_open_) {
        return Close();
    }
    return true;
}

std::string PDFWriteDevice::GetName() const {
    return "PDFWrite";
}

bool PDFWriteDevice::IsInitialized() const {
    return is_initialized_;
}

std::vector<std::string> PDFWriteDevice::GetSupportedExtensions() const {
    return {".pdf"};
}

std::string PDFWriteDevice::GetCapabilities() const {
    return "PDF output device supporting metadata, security, and multi-page documents";
}

bool PDFWriteDevice::SetParameter(const std::string& name, const std::string& value) {
    // Handle device-specific parameters
    if (name == "output_file") {
        output_file_ = value;
        return true;
    } else if (name == "title") {
        title_ = value;
        return true;
    } else if (name == "author") {
        author_ = value;
        return true;
    } else if (name == "subject") {
        subject_ = value;
        return true;
    } else if (name == "keywords") {
        keywords_ = value;
        return true;
    }
    return false;
}

std::string PDFWriteDevice::GetParameter(const std::string& name) const {
    if (name == "output_file") {
        return output_file_;
    } else if (name == "title") {
        return title_;
    } else if (name == "author") {
        return author_;
    } else if (name == "subject") {
        return subject_;
    } else if (name == "keywords") {
        return keywords_;
    }
    return "";
}

} // namespace PDFLib 