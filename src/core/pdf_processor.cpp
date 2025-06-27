/**
 * @file pdf_processor.cpp
 * @brief Main PDF processor implementation
 */

#include "pdf_processor.h"
#include "command_parser.h"
#include "device_manager.h"
#include "postscript_interpreter.h"
#include "resource_manager.h"
#include "error_handler.h"
#include "pdf_engine.h"
#include "postscript_parser.h"
#include "pdf_generator.h"
#include <memory>
#include <algorithm>

namespace PDFLib {

class PDFProcessor::Impl {
public:
    Impl() : initialized_(false), progress_callback_(nullptr) {
        // Initialize components
        command_parser_ = std::make_unique<CommandParser>();
        device_manager_ = std::make_unique<DeviceManager>();
        postscript_interpreter_ = std::make_unique<PostScriptInterpreter>();
        resource_manager_ = std::make_unique<ResourceManager>();
        pdf_engine_ = std::make_unique<PDFEngine>();
        
        // Initialize rendering backend components
        postscript_parser_ = std::make_unique<PostScriptParser>();
        pdf_generator_ = std::make_unique<PDFGenerator>();
        
        // Error callback will be set later if needed
        // Removed problematic callback that could cause infinite loops
    }
    
    ~Impl() {
        Cleanup();
    }
    
    bool Initialize(const std::vector<std::string>& arguments) {
        if (initialized_) {
            ErrorHandler::GetInstance().SetError(-10, "Processor already initialized");
            return false;
        }
        
        // Parse command-line arguments
        if (!command_parser_->ParseArguments(arguments, options_)) {
            ErrorHandler::GetInstance().SetError(-1, "Failed to parse command-line arguments");
            return false;
        }
        
        // Validate options
        if (!command_parser_->ValidateOptions(options_)) {
            return false;
        }
        
        // Initialize device manager
        if (!device_manager_->Initialize(options_, ErrorHandler::GetInstance())) {
            return false;
        }
        
        // Initialize PostScript interpreter
        if (!postscript_interpreter_->Initialize(ErrorHandler::GetInstance())) {
            return false;
        }
        
        // Initialize PDF engine
        if (!pdf_engine_->Initialize(options_, ErrorHandler::GetInstance())) {
            return false;
        }
        
        // Initialize resource manager
        if (!resource_manager_->Initialize(ErrorHandler::GetInstance())) {
            return false;
        }
        
        // Initialize rendering backend
        if (!pdf_generator_->Initialize(options_, ErrorHandler::GetInstance())) {
            return false;
        }
        
        initialized_ = true;
        ErrorHandler::GetInstance().Log("PDF processor initialized successfully", ErrorSeverity::INFO);
        return true;
    }
    
    bool Initialize(const ProcessingOptions& options) {
        if (initialized_) {
            ErrorHandler::GetInstance().SetError(-10, "Processor already initialized");
            return false;
        }
        
        options_ = options;
        
        // Validate options
        if (!command_parser_->ValidateOptions(options_)) {
            return false;
        }
        
        // Initialize device manager
        if (!device_manager_->Initialize(options_, ErrorHandler::GetInstance())) {
            return false;
        }
        
        // Initialize PostScript interpreter
        if (!postscript_interpreter_->Initialize(ErrorHandler::GetInstance())) {
            return false;
        }
        
        // Initialize PDF engine
        if (!pdf_engine_->Initialize(options_, ErrorHandler::GetInstance())) {
            return false;
        }
        
        // Initialize resource manager
        if (!resource_manager_->Initialize(ErrorHandler::GetInstance())) {
            return false;
        }
        
        // Initialize rendering backend
        if (!pdf_generator_->Initialize(options_, ErrorHandler::GetInstance())) {
            return false;
        }
        
        initialized_ = true;
        ErrorHandler::GetInstance().Log("PDF processor initialized successfully", ErrorSeverity::INFO);
        return true;
    }
    
    bool Process() {
        if (!initialized_) {
            ErrorHandler::GetInstance().SetError(-9, "Processor not initialized");
            return false;
        }
        
        try {
            ErrorHandler::GetInstance().Log("Starting PDF processing", ErrorSeverity::INFO);
            
            // Process PostScript commands first
            if (!options_.postscript_commands.empty()) {
                if (!ProcessPostScriptCommands()) {
                    return false;
                }
            }
            
            // Process input files
            if (!options_.input_files.empty()) {
                if (!ProcessInputFiles()) {
                    return false;
                }
            }
            
            // Execute device operations
            if (!device_manager_->Process(*pdf_engine_, ErrorHandler::GetInstance())) {
                return false;
            }
            
            ErrorHandler::GetInstance().Log("PDF processing completed successfully", ErrorSeverity::INFO);
            return true;
            
        } catch (const std::exception& e) {
            ErrorHandler::GetInstance().SetError(-8, "Exception during processing: " + std::string(e.what()));
            return false;
        }
    }
    
    void SetProgressCallback(ProgressCallback callback) {
        progress_callback_ = callback;
    }
    
    ErrorInfo GetLastError() const {
        return ErrorHandler::GetInstance().GetError();
    }
    
    std::string GetLastErrorString() const {
        return ErrorHandler::GetInstance().GetErrorString();
    }
    
    void Cleanup() {
        if (initialized_) {
            ErrorHandler::GetInstance().Log("Cleaning up PDF processor", ErrorSeverity::INFO);
            
            // Cleanup components in reverse order
            if (pdf_engine_) {
                pdf_engine_->Cleanup();
            }
            
            if (postscript_interpreter_) {
                postscript_interpreter_->Cleanup();
            }
            
            if (device_manager_) {
                device_manager_->Cleanup();
            }
            
            if (resource_manager_) {
                resource_manager_->Cleanup();
            }
            
            initialized_ = false;
        }
    }
    
    bool IsInitialized() const {
        return initialized_;
    }
    
    ProcessingOptions GetOptions() const {
        return options_;
    }

private:
    bool ProcessPostScriptCommands() {
        ErrorHandler::GetInstance().Log("Processing PostScript commands", ErrorSeverity::INFO);
        
        for (const auto& command : options_.postscript_commands) {
            if (!postscript_interpreter_->ExecuteCommand(command, ErrorHandler::GetInstance())) {
                ErrorHandler::GetInstance().SetError(-12, "Failed to execute PostScript command: " + command);
                return false;
            }
        }
        
        return true;
    }
    
    bool ProcessInputFiles() {
        ErrorHandler::GetInstance().Log("Processing input files", ErrorSeverity::INFO);
        
        int total_files = static_cast<int>(options_.input_files.size());
        int current_file = 0;
        
        for (const auto& input_file : options_.input_files) {
            current_file++;
            
            if (progress_callback_) {
                progress_callback_(current_file, total_files, "Processing file: " + input_file);
            }
            
            ErrorHandler::GetInstance().Log("Processing file: " + input_file, ErrorSeverity::INFO);
            
            // Use the new rendering backend for PostScript files
            std::string extension = input_file.substr(input_file.find_last_of('.') + 1);
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            
            if (extension == "ps" || extension == "eps") {
                // Parse PostScript file with new parser
                if (!postscript_parser_->ParseFile(input_file, ErrorHandler::GetInstance())) {
                    ErrorHandler::GetInstance().SetError(-4, "Failed to parse PostScript file: " + input_file);
                    return false;
                }
                
                // Generate PDF from parsed PostScript
                if (!pdf_generator_->CreatePDF(*postscript_parser_, options_.output_file, 
                                              ErrorHandler::GetInstance())) {
                    ErrorHandler::GetInstance().SetError(-5, "Failed to generate PDF from: " + input_file);
                    return false;
                }
                
                ErrorHandler::GetInstance().Log("Successfully converted PostScript to PDF", ErrorSeverity::INFO);
                
            } else {
                // Fallback to original engine for other file types
                if (!pdf_engine_->LoadFile(input_file, ErrorHandler::GetInstance())) {
                    ErrorHandler::GetInstance().SetError(-4, "Failed to load file: " + input_file);
                    return false;
                }
                
                // Apply PostScript transformations if any
                if (postscript_interpreter_->HasTransformations()) {
                    if (!pdf_engine_->ApplyTransformations(*postscript_interpreter_, ErrorHandler::GetInstance())) {
                        ErrorHandler::GetInstance().SetError(-12, "Failed to apply PostScript transformations");
                        return false;
                    }
                }
            }
        }
        
        return true;
    }
    
    bool initialized_;
    ProcessingOptions options_;
    ProgressCallback progress_callback_;
    
    std::unique_ptr<CommandParser> command_parser_;
    std::unique_ptr<DeviceManager> device_manager_;
    std::unique_ptr<PostScriptInterpreter> postscript_interpreter_;
    std::unique_ptr<ResourceManager> resource_manager_;
    std::unique_ptr<PDFEngine> pdf_engine_;
    
    // Rendering backend components
    std::unique_ptr<PostScriptParser> postscript_parser_;
    std::unique_ptr<PDFGenerator> pdf_generator_;
};

// PDFProcessor implementation
PDFProcessor::PDFProcessor() : pImpl(std::make_unique<Impl>()) {}

PDFProcessor::~PDFProcessor() = default;

bool PDFProcessor::Initialize(const std::vector<std::string>& arguments) {
    return pImpl->Initialize(arguments);
}

bool PDFProcessor::Initialize(const ProcessingOptions& options) {
    return pImpl->Initialize(options);
}

bool PDFProcessor::Process() {
    return pImpl->Process();
}

void PDFProcessor::SetProgressCallback(ProgressCallback callback) {
    pImpl->SetProgressCallback(callback);
}

ErrorInfo PDFProcessor::GetLastError() const {
    return pImpl->GetLastError();
}

std::string PDFProcessor::GetLastErrorString() const {
    return pImpl->GetLastErrorString();
}

void PDFProcessor::Cleanup() {
    pImpl->Cleanup();
}

bool PDFProcessor::IsInitialized() const {
    return pImpl->IsInitialized();
}

ProcessingOptions PDFProcessor::GetOptions() const {
    return pImpl->GetOptions();
}

} // namespace PDFLib 