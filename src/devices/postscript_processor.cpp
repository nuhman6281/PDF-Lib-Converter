/**
 * @file postscript_processor.cpp
 * @brief PostScript processor device implementation
 * @version 1.0.0
 * @author PDF_LIB Team
 */

#include "postscript_processor.h"
#include "../core/pdf_engine.h"
#include "../core/resource_manager.h"
#include "../core/postscript_interpreter.h"
#include "../core/error_handler.h"
#include "../utils/file_utils.h"
#include "../utils/string_utils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

namespace PDFLib {

PostScriptProcessor::PostScriptProcessor(std::shared_ptr<PDFEngine> engine, 
                                         std::shared_ptr<ResourceManager> resource_manager)
    : pdf_engine_(engine)
    , resource_manager_(resource_manager)
    , is_open_(false)
    , is_initialized_(false)
    , bbox_llx_(0.0)
    , bbox_lly_(0.0)
    , bbox_urx_(612.0)
    , bbox_ury_(792.0)
    , bbox_valid_(false) {
}

PostScriptProcessor::~PostScriptProcessor() {
    Close();
}

bool PostScriptProcessor::Initialize(const ProcessingOptions& options) {
    if (is_initialized_) {
        ErrorHandler::GetInstance().SetError(-1, "Device already initialized");
        return false;
    }
    
    if (!InitializeInterpreter()) {
        return false;
    }
    
    is_initialized_ = true;
    return true;
}

bool PostScriptProcessor::Open(const std::string& input_file) {
    if (!is_initialized_) {
        ErrorHandler::GetInstance().SetError(-1, "Device not initialized");
        return false;
    }
    
    if (is_open_) {
        Close();
    }
    
    if (!Utils::FileExists(input_file)) {
        ErrorHandler::GetInstance().SetError(-1, "Input file not found: " + input_file);
        return false;
    }
    
    input_file_ = input_file;
    
    // Parse file header to extract metadata
    if (!ParseHeader()) {
        return false;
    }
    
    is_open_ = true;
    return true;
}

bool PostScriptProcessor::Close() {
    if (!is_open_) {
        return true;
    }
    
    if (interpreter_) {
        // interpreter_->Reset(); // Method not implemented
    }
    
    is_open_ = false;
    bbox_valid_ = false;
    return true;
}

bool PostScriptProcessor::Process() {
    if (!is_open_) {
        ErrorHandler::GetInstance().SetError(-1, "Device not open");
        return false;
    }
    
    if (!interpreter_) {
        ErrorHandler::GetInstance().SetError(-1, "PostScript interpreter not initialized");
        return false;
    }
    
    // Read and process the PostScript file
    std::ifstream file(input_file_);
    if (!file.is_open()) {
        ErrorHandler::GetInstance().SetError(-1, "Failed to open input file: " + input_file_);
        return false;
    }
    
    std::string line;
    std::vector<std::string> commands;
    
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '%') {
            continue;
        }
        
        // Add line to commands
        commands.push_back(line);
    }
    
    file.close();
    
    // Execute the commands
    return ExecuteCommands(commands);
}

bool PostScriptProcessor::ExecuteCommands(const std::vector<std::string>& commands) {
    if (!interpreter_) {
        ErrorHandler::GetInstance().SetError(-1, "PostScript interpreter not initialized");
        return false;
    }
    
    for (const auto& command : commands) {
        if (!interpreter_->ExecuteCommand(command, ErrorHandler::GetInstance())) {
            ErrorHandler::GetInstance().SetError(-1, "Failed to execute PostScript command: " + command);
            return false;
        }
    }
    
    return true;
}

bool PostScriptProcessor::GetBoundingBox(double& llx, double& lly, double& urx, double& ury) {
    if (!bbox_valid_) {
        return false;
    }
    
    llx = bbox_llx_;
    lly = bbox_lly_;
    urx = bbox_urx_;
    ury = bbox_ury_;
    
    return true;
}

void PostScriptProcessor::SetVariables(const std::map<std::string, std::string>& variables) {
    variables_ = variables;
    
    // Set variables in interpreter
    if (interpreter_) {
        for (const auto& var : variables_) {
            // interpreter_->SetVariable(var.first, var.second); // Method not implemented
        }
    }
}

bool PostScriptProcessor::IsOpen() const {
    return is_open_;
}

bool PostScriptProcessor::IsEPSFile(const std::string& filepath) {
    std::string extension = Utils::GetFileExtension(filepath);
    if (Utils::ToLower(extension) == "eps") {
        return true;
    }
    
    // Check file header for EPS signature
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    char buffer[256];
    file.read(buffer, sizeof(buffer) - 1);
    buffer[file.gcount()] = '\0';
    
    file.close();
    
    std::string content(buffer);
    
    // Check for EPS header
    if (content.find("%!PS-Adobe-3.0 EPSF") != std::string::npos ||
        content.find("%!PS-Adobe-2.0 EPSF") != std::string::npos) {
        return true;
    }
    
    return false;
}

bool PostScriptProcessor::IsPostScriptFile(const std::string& filepath) {
    std::string extension = Utils::GetFileExtension(filepath);
    std::string lower_ext = Utils::ToLower(extension);
    
    if (lower_ext == "ps" || lower_ext == "eps" || lower_ext == "prn") {
        return true;
    }
    
    // Check file header for PostScript signature
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    char buffer[256];
    file.read(buffer, sizeof(buffer) - 1);
    buffer[file.gcount()] = '\0';
    
    file.close();
    
    std::string content(buffer);
    
    // Check for PostScript header
    if (content.find("%!PS-Adobe") != std::string::npos) {
        return true;
    }
    
    return false;
}

bool PostScriptProcessor::ParseHeader() {
    if (input_file_.empty()) {
        return false;
    }
    
    std::ifstream file(input_file_);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int line_count = 0;
    const int max_header_lines = 50; // Limit header parsing to first 50 lines
    
    while (std::getline(file, line) && line_count < max_header_lines) {
        line_count++;
        
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        // Extract bounding box information
        if (line.find("%%BoundingBox:") != std::string::npos) {
            ExtractBoundingBoxFromLine(line);
        }
        
        // Extract other metadata
        if (line.find("%%Title:") != std::string::npos) {
            // Extract title
        }
        
        if (line.find("%%Creator:") != std::string::npos) {
            // Extract creator
        }
        
        if (line.find("%%CreationDate:") != std::string::npos) {
            // Extract creation date
        }
    }
    
    file.close();
    return true;
}

bool PostScriptProcessor::ExtractBoundingBox() {
    if (input_file_.empty()) {
        return false;
    }
    
    std::ifstream file(input_file_);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("%%BoundingBox:") != std::string::npos) {
            file.close();
            return ExtractBoundingBoxFromLine(line);
        }
    }
    
    file.close();
    return false;
}

bool PostScriptProcessor::ExtractBoundingBoxFromLine(const std::string& line) {
    // Parse %%BoundingBox: llx lly urx ury
    std::regex bbox_regex(R"(%%BoundingBox:\s*([+-]?\d+(?:\.\d+)?)\s+([+-]?\d+(?:\.\d+)?)\s+([+-]?\d+(?:\.\d+)?)\s+([+-]?\d+(?:\.\d+)?))");
    std::smatch match;
    
    if (std::regex_search(line, match, bbox_regex)) {
        try {
            bbox_llx_ = std::stod(match[1].str());
            bbox_lly_ = std::stod(match[2].str());
            bbox_urx_ = std::stod(match[3].str());
            bbox_ury_ = std::stod(match[4].str());
            bbox_valid_ = true;
            return true;
        } catch (const std::exception& e) {
            ErrorHandler::GetInstance().SetError(-1, "Failed to parse bounding box: " + std::string(e.what()));
            return false;
        }
    }
    
    return false;
}

bool PostScriptProcessor::InitializeInterpreter() {
    interpreter_ = std::make_shared<PostScriptInterpreter>();
    if (!interpreter_) {
        ErrorHandler::GetInstance().SetError(-1, "Failed to create PostScript interpreter");
        return false;
    }
    
    if (!interpreter_->Initialize(ErrorHandler::GetInstance())) {
        ErrorHandler::GetInstance().SetError(-1, "Failed to initialize PostScript interpreter");
        return false;
    }
    
    return true;
}

bool PostScriptProcessor::Open() {
    return Open("");
}

bool PostScriptProcessor::WritePage(const std::vector<uint8_t>& page_data, int page_number) {
    // For PostScript processor, this would typically process the input
    // and delegate to another device for output
    return true;
}

bool PostScriptProcessor::Finalize() {
    return Process();
}

std::string PostScriptProcessor::GetName() const {
    return "PostScriptProcessor";
}

bool PostScriptProcessor::IsInitialized() const {
    return is_initialized_;
}

std::vector<std::string> PostScriptProcessor::GetSupportedExtensions() const {
    return {".ps", ".eps"};
}

std::string PostScriptProcessor::GetCapabilities() const {
    return "PostScript and EPS file processor with interpreter support";
}

bool PostScriptProcessor::SetParameter(const std::string& name, const std::string& value) {
    if (name == "input_file") {
        input_file_ = value;
        return true;
    }
    variables_[name] = value;
    return true;
}

std::string PostScriptProcessor::GetParameter(const std::string& name) const {
    if (name == "input_file") {
        return input_file_;
    }
    auto it = variables_.find(name);
    if (it != variables_.end()) {
        return it->second;
    }
    return "";
}

} // namespace PDFLib
