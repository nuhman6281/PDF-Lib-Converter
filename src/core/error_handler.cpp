/**
 * @file error_handler.cpp
 * @brief Error handling and management implementation
 */

#include "error_handler.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <mutex>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

namespace PDFLib {

class ErrorHandler::Impl {
public:
    Impl() : logging_enabled_(true), error_callback_(nullptr) {
        // Set default log file
        log_file_ = "pdf_lib.log";
    }
    
    ~Impl() {
        if (log_stream_.is_open()) {
            log_stream_.close();
        }
    }
    
    void SetError(int code, const std::string& message, 
                  ErrorSeverity severity, const ErrorContext& context) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        current_error_.code = code;
        current_error_.message = message;
        current_error_.file = context.file_name;
        current_error_.line = context.line_number;
        
        // Log the error
        std::stringstream ss;
        ss << "ERROR [" << GetSeverityString(severity) << "] ";
        ss << "Code: " << code << ", Message: " << message;
        if (!context.file_name.empty()) {
            ss << ", File: " << context.file_name << ":" << context.line_number;
        }
        if (!context.function_name.empty()) {
            ss << ", Function: " << context.function_name;
        }
        if (!context.additional_info.empty()) {
            ss << ", Info: " << context.additional_info;
        }
        
        Log(ss.str(), severity);
        
        // Call error callback if set
        if (error_callback_) {
            error_callback_(current_error_);
        }
    }
    
    void ClearError() {
        std::lock_guard<std::mutex> lock(mutex_);
        current_error_ = ErrorInfo{};
    }
    
    ErrorInfo GetError() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return current_error_;
    }
    
    std::string GetErrorString() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (current_error_.code == 0) {
            return "No error";
        }
        
        std::stringstream ss;
        ss << "Error " << current_error_.code << ": " << current_error_.message;
        if (!current_error_.file.empty()) {
            ss << " (at " << current_error_.file << ":" << current_error_.line << ")";
        }
        return ss.str();
    }
    
    bool HasError() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return current_error_.code != 0;
    }
    
    int GetErrorCode() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return current_error_.code;
    }
    
    void SetErrorCallback(std::function<void(const ErrorInfo&)> callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        error_callback_ = callback;
    }
    
    void Log(const std::string& message, ErrorSeverity severity) {
        if (!logging_enabled_) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::string timestamp = GetCurrentTimestamp();
        std::string severity_str = GetSeverityString(severity);
        
        std::stringstream ss;
        ss << "[" << timestamp << "] [" << severity_str << "] " << message;
        
        // Write to log file
        if (!log_file_.empty()) {
            if (!log_stream_.is_open()) {
                log_stream_.open(log_file_, std::ios::app);
            }
            if (log_stream_.is_open()) {
                log_stream_ << ss.str() << std::endl;
                log_stream_.flush();
            }
        }
        
        // Also write to console for errors and warnings
        if (severity == ErrorSeverity::ERROR || severity == ErrorSeverity::FATAL) {
            std::cerr << ss.str() << std::endl;
        } else if (severity == ErrorSeverity::WARNING) {
            std::cout << ss.str() << std::endl;
        }
    }
    
    void SetLoggingEnabled(bool enabled) {
        std::lock_guard<std::mutex> lock(mutex_);
        logging_enabled_ = enabled;
    }
    
    void SetLogFile(const std::string& log_file) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (log_stream_.is_open()) {
            log_stream_.close();
        }
        log_file_ = log_file;
    }
    
    ErrorContext GetErrorContext() const {
        std::lock_guard<std::mutex> lock(mutex_);
        ErrorContext context;
        context.file_name = current_error_.file;
        context.line_number = current_error_.line;
        return context;
    }

private:
    std::string GetCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
    std::string GetSeverityString(ErrorSeverity severity) const {
        switch (severity) {
            case ErrorSeverity::INFO: return "INFO";
            case ErrorSeverity::WARNING: return "WARN";
            case ErrorSeverity::ERROR: return "ERROR";
            case ErrorSeverity::FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }
    
    mutable std::mutex mutex_;
    ErrorInfo current_error_;
    bool logging_enabled_;
    std::string log_file_;
    std::ofstream log_stream_;
    std::function<void(const ErrorInfo&)> error_callback_;
};

// ErrorHandler implementation
ErrorHandler& ErrorHandler::GetInstance() {
    static ErrorHandler instance;
    return instance;
}

ErrorHandler::ErrorHandler() : pImpl(std::make_unique<Impl>()) {}

ErrorHandler::~ErrorHandler() = default;

void ErrorHandler::SetError(int code, const std::string& message, 
                           ErrorSeverity severity, const ErrorContext& context) {
    pImpl->SetError(code, message, severity, context);
}

void ErrorHandler::SetError(int code, const std::string& message, 
                           const std::string& file, int line, ErrorSeverity severity) {
    ErrorContext context;
    context.file_name = file;
    context.line_number = line;
    pImpl->SetError(code, message, severity, context);
}

void ErrorHandler::ClearError() {
    pImpl->ClearError();
}

ErrorInfo ErrorHandler::GetError() const {
    return pImpl->GetError();
}

std::string ErrorHandler::GetErrorString() const {
    return pImpl->GetErrorString();
}

bool ErrorHandler::HasError() const {
    return pImpl->HasError();
}

int ErrorHandler::GetErrorCode() const {
    return pImpl->GetErrorCode();
}

void ErrorHandler::SetErrorCallback(std::function<void(const ErrorInfo&)> callback) {
    pImpl->SetErrorCallback(callback);
}

void ErrorHandler::Log(const std::string& message, ErrorSeverity severity) {
    pImpl->Log(message, severity);
}

void ErrorHandler::SetLoggingEnabled(bool enabled) {
    pImpl->SetLoggingEnabled(enabled);
}

void ErrorHandler::SetLogFile(const std::string& log_file) {
    pImpl->SetLogFile(log_file);
}

ErrorContext ErrorHandler::GetErrorContext() const {
    return pImpl->GetErrorContext();
}

} // namespace PDFLib 