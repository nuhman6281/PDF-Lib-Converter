/**
 * @file pdf_lib.h
 * @brief Main header file for PDF_LIB - A lightweight Ghostscript alternative
 * @version 1.0.0
 * @author PDF_LIB Team
 * 
 * This library provides core Ghostscript functionality focused on PDF processing
 * and printing operations, serving as a lightweight alternative to Ghostscript.
 */

#ifndef PDF_LIB_H
#define PDF_LIB_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

#ifdef _WIN32
    #ifdef PDF_LIB_EXPORTS
        #define PDF_LIB_API __declspec(dllexport)
    #else
        #define PDF_LIB_API __declspec(dllimport)
    #endif
#else
    #define PDF_LIB_API __attribute__((visibility("default")))
#endif

namespace PDFLib {

// Forward declarations
class PDFProcessor;
class Device;
class ErrorHandler;

/**
 * @brief PDF processing quality settings
 */
enum class PDFQuality {
    SCREEN,     ///< Screen quality (72 dpi)
    DEFAULT,    ///< Default quality
    EBOOK,      ///< E-book quality
    PRINTER,    ///< Printer quality (300 dpi)
    PREPRESS    ///< Prepress quality (300 dpi)
};

/**
 * @brief Paper size enumeration
 */
enum class PaperSize {
    CUSTOM,     ///< Custom paper size
    A4,         ///< A4 (210x297 mm)
    LETTER,     ///< Letter (8.5x11 inches)
    LEGAL,      ///< Legal (8.5x14 inches)
    A3,         ///< A3 (297x420 mm)
    A5,         ///< A5 (148x210 mm)
    EXECUTIVE   ///< Executive (7.25x10.5 inches)
};

/**
 * @brief Device types supported by the library
 */
enum class DeviceType {
    PDF_WRITE,      ///< PDF output device
    MSWIN_PRINTER,  ///< Windows printer device
    POSTSCRIPT      ///< PostScript processor
};

/**
 * @brief Processing options structure
 */
struct ProcessingOptions {
    bool batch_mode = false;           ///< Process files and exit
    bool no_pause = false;             ///< Disable pausing between pages
    bool quiet = false;                ///< Suppress output messages
    bool safer_mode = true;            ///< Enable safe mode operations
    bool printed = false;              ///< Mark output as printed
    bool empty_stack = false;          ///< Start with empty stack
    
    PDFQuality quality = PDFQuality::DEFAULT;
    double compatibility_level = 1.7;  ///< PDF compatibility level
    int num_copies = 1;                ///< Number of copies for printing
    
    PaperSize paper_size = PaperSize::A4;
    double custom_width_points = 0.0;  ///< Custom width in points
    double custom_height_points = 0.0; ///< Custom height in points
    
    double left_margin = 0.0;          ///< Left margin in points
    double right_margin = 0.0;         ///< Right margin in points
    double top_margin = 0.0;           ///< Top margin in points
    double bottom_margin = 0.0;        ///< Bottom margin in points
    
    double page_offset_x = 0.0;        ///< Page offset X in points
    double page_offset_y = 0.0;        ///< Page offset Y in points
    
    std::string output_file;           ///< Output file path
    std::string device_name;           ///< Device name
    std::string printer_name;          ///< Printer name for Windows printing
    
    std::vector<std::string> input_files;  ///< Input file paths
    std::vector<std::string> postscript_commands; ///< PostScript commands to execute
};

/**
 * @brief Error information structure
 */
struct ErrorInfo {
    int code;                          ///< Error code
    std::string message;               ///< Error message
    std::string file;                  ///< File where error occurred
    int line;                          ///< Line number where error occurred
};

/**
 * @brief Progress callback function type
 */
using ProgressCallback = std::function<void(int current, int total, const std::string& status)>;

/**
 * @brief Main PDF processor class
 * 
 * This class provides the core functionality for PDF processing operations,
 * including PDF creation, manipulation, and printing.
 */
class PDF_LIB_API PDFProcessor {
public:
    /**
     * @brief Constructor
     */
    PDFProcessor();
    
    /**
     * @brief Destructor
     */
    ~PDFProcessor();
    
    /**
     * @brief Initialize the processor with command-line style arguments
     * @param arguments Vector of command-line arguments
     * @return true if initialization successful, false otherwise
     */
    bool Initialize(const std::vector<std::string>& arguments);
    
    /**
     * @brief Initialize the processor with processing options
     * @param options Processing options structure
     * @return true if initialization successful, false otherwise
     */
    bool Initialize(const ProcessingOptions& options);
    
    /**
     * @brief Execute the processing operation
     * @return true if processing successful, false otherwise
     */
    bool Process();
    
    /**
     * @brief Set progress callback function
     * @param callback Progress callback function
     */
    void SetProgressCallback(ProgressCallback callback);
    
    /**
     * @brief Get the last error information
     * @return Error information structure
     */
    ErrorInfo GetLastError() const;
    
    /**
     * @brief Get error message as string
     * @return Error message string
     */
    std::string GetLastErrorString() const;
    
    /**
     * @brief Clean up resources
     */
    void Cleanup();
    
    /**
     * @brief Check if processor is initialized
     * @return true if initialized, false otherwise
     */
    bool IsInitialized() const;
    
    /**
     * @brief Get current processing options
     * @return Current processing options
     */
    ProcessingOptions GetOptions() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Utility functions for PDF processing
 */
namespace Utils {
    
    /**
     * @brief Parse command-line arguments into ProcessingOptions
     * @param arguments Command-line arguments
     * @return ProcessingOptions structure
     */
    PDF_LIB_API ProcessingOptions ParseArguments(const std::vector<std::string>& arguments);
    
    /**
     * @brief Convert PDFQuality to string
     * @param quality PDF quality setting
     * @return Quality string representation
     */
    PDF_LIB_API std::string QualityToString(PDFQuality quality);
    
    /**
     * @brief Convert string to PDFQuality
     * @param str Quality string
     * @return PDFQuality enumeration
     */
    PDF_LIB_API PDFQuality StringToQuality(const std::string& str);
    
    /**
     * @brief Convert PaperSize to string
     * @param size Paper size
     * @return Paper size string representation
     */
    PDF_LIB_API std::string PaperSizeToString(PaperSize size);
    
    /**
     * @brief Convert string to PaperSize
     * @param str Paper size string
     * @return PaperSize enumeration
     */
    PDF_LIB_API PaperSize StringToPaperSize(const std::string& str);
    
    /**
     * @brief Get paper dimensions in points
     * @param size Paper size
     * @param width Output width in points
     * @param height Output height in points
     */
    PDF_LIB_API void GetPaperDimensions(PaperSize size, double& width, double& height);
    
    /**
     * @brief Check if file exists
     * @param filepath File path to check
     * @return true if file exists, false otherwise
     */
    PDF_LIB_API bool FileExists(const std::string& filepath);
    
    /**
     * @brief Get file extension
     * @param filepath File path
     * @return File extension (lowercase)
     */
    PDF_LIB_API std::string GetFileExtension(const std::string& filepath);
    
    /**
     * @brief Create temporary file
     * @param prefix File prefix
     * @param extension File extension
     * @return Temporary file path
     */
    PDF_LIB_API std::string CreateTempFile(const std::string& prefix = "pdf_lib_", 
                                          const std::string& extension = ".tmp");
    
    /**
     * @brief Delete file
     * @param filepath File path to delete
     * @return true if deletion successful, false otherwise
     */
    PDF_LIB_API bool DeleteFile(const std::string& filepath);
}

} // namespace PDFLib

#endif // PDF_LIB_H 