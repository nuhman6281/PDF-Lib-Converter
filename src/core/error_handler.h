/**
 * @file error_handler.h
 * @brief Error handling and management for PDF_LIB
 */

#ifndef PDF_LIB_ERROR_HANDLER_H
#define PDF_LIB_ERROR_HANDLER_H

#include <string>
#include <memory>
#include <functional>
#include "../include/pdf_lib.h"

namespace PDFLib {

/**
 * @brief Error severity levels
 */
enum class ErrorSeverity {
    INFO,       ///< Informational message
    WARNING,    ///< Warning message
    ERROR,      ///< Error message
    FATAL       ///< Fatal error
};

/**
 * @brief Error context information
 */
struct ErrorContext {
    std::string function_name;     ///< Function where error occurred
    std::string file_name;         ///< File where error occurred
    int line_number;               ///< Line number where error occurred
    std::string additional_info;   ///< Additional error information
};

/**
 * @brief Error handler class (Singleton)
 * 
 * Provides comprehensive error handling and logging functionality
 * for the PDF library.
 */
class ErrorHandler {
public:
    /**
     * @brief Get singleton instance
     * @return Reference to the singleton instance
     */
    static ErrorHandler& GetInstance();
    
    /**
     * @brief Destructor
     */
    ~ErrorHandler();
    
    /**
     * @brief Set error information
     * @param code Error code
     * @param message Error message
     * @param severity Error severity
     * @param context Error context
     */
    void SetError(int code, const std::string& message, 
                  ErrorSeverity severity = ErrorSeverity::ERROR,
                  const ErrorContext& context = ErrorContext{});
    
    /**
     * @brief Set error information with file and line
     * @param code Error code
     * @param message Error message
     * @param file File name
     * @param line Line number
     * @param severity Error severity
     */
    void SetError(int code, const std::string& message, 
                  const std::string& file, int line,
                  ErrorSeverity severity = ErrorSeverity::ERROR);
    
    /**
     * @brief Clear current error
     */
    void ClearError();
    
    /**
     * @brief Get current error information
     * @return Error information structure
     */
    ErrorInfo GetError() const;
    
    /**
     * @brief Get error message as string
     * @return Error message string
     */
    std::string GetErrorString() const;
    
    /**
     * @brief Check if there is an error
     * @return true if error exists, false otherwise
     */
    bool HasError() const;
    
    /**
     * @brief Get error code
     * @return Error code
     */
    int GetErrorCode() const;
    
    /**
     * @brief Set error callback function
     * @param callback Error callback function
     */
    void SetErrorCallback(std::function<void(const ErrorInfo&)> callback);
    
    /**
     * @brief Log message
     * @param message Message to log
     * @param severity Message severity
     */
    void Log(const std::string& message, ErrorSeverity severity = ErrorSeverity::INFO);
    
    /**
     * @brief Enable/disable logging
     * @param enabled true to enable logging, false to disable
     */
    void SetLoggingEnabled(bool enabled);
    
    /**
     * @brief Set log file path
     * @param log_file Log file path
     */
    void SetLogFile(const std::string& log_file);
    
    /**
     * @brief Get error context
     * @return Error context structure
     */
    ErrorContext GetErrorContext() const;

private:
    /**
     * @brief Private constructor for singleton
     */
    ErrorHandler();
    
    /**
     * @brief Copy constructor (deleted)
     */
    ErrorHandler(const ErrorHandler&) = delete;
    
    /**
     * @brief Assignment operator (deleted)
     */
    ErrorHandler& operator=(const ErrorHandler&) = delete;
    
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Error helper macros
 */
#define PDF_LIB_SET_ERROR(handler, code, message) \
    (handler).SetError(code, message, __FILE__, __LINE__)

#define PDF_LIB_SET_ERROR_IF(condition, handler, code, message) \
    do { \
        if (condition) { \
            PDF_LIB_SET_ERROR(handler, code, message); \
        } \
    } while(0)

#define PDF_LIB_RETURN_ERROR_IF(condition, handler, code, message) \
    do { \
        if (condition) { \
            PDF_LIB_SET_ERROR(handler, code, message); \
            return false; \
        } \
    } while(0)

#define PDF_LIB_RETURN_NULL_IF(condition, handler, code, message) \
    do { \
        if (condition) { \
            PDF_LIB_SET_ERROR(handler, code, message); \
            return nullptr; \
        } \
    } while(0)

} // namespace PDFLib

#endif // PDF_LIB_ERROR_HANDLER_H 