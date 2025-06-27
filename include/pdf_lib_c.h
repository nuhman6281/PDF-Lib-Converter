/**
 * @file pdf_lib_c.h
 * @brief C API wrapper for PDF_LIB - A lightweight Ghostscript alternative
 * @version 1.0.0
 * @author PDF_LIB Team
 * 
 * This header provides a C-style API wrapper for the PDF_LIB library,
 * enabling easy integration with other languages like C# via P/Invoke.
 */

#ifndef PDF_LIB_C_H
#define PDF_LIB_C_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
    #ifdef PDF_LIB_C_EXPORTS
        #define PDF_LIB_C_API __declspec(dllexport)
    #else
        #define PDF_LIB_C_API __declspec(dllimport)
    #endif
#else
    #define PDF_LIB_C_API __attribute__((visibility("default")))
#endif

/**
 * @brief Error codes
 */
typedef enum {
    PDF_LIB_SUCCESS = 0,
    PDF_LIB_ERROR_INVALID_ARGUMENT = -1,
    PDF_LIB_ERROR_FILE_NOT_FOUND = -2,
    PDF_LIB_ERROR_FILE_ACCESS_DENIED = -3,
    PDF_LIB_ERROR_INVALID_PDF = -4,
    PDF_LIB_ERROR_PRINTER_NOT_FOUND = -5,
    PDF_LIB_ERROR_PRINTER_ACCESS_DENIED = -6,
    PDF_LIB_ERROR_OUT_OF_MEMORY = -7,
    PDF_LIB_ERROR_INTERNAL = -8,
    PDF_LIB_ERROR_NOT_INITIALIZED = -9,
    PDF_LIB_ERROR_ALREADY_INITIALIZED = -10,
    PDF_LIB_ERROR_UNSUPPORTED_FORMAT = -11,
    PDF_LIB_ERROR_POSTSCRIPT_ERROR = -12
} pdf_lib_error_code_t;

/**
 * @brief PDF quality settings
 */
typedef enum {
    PDF_LIB_QUALITY_SCREEN = 0,
    PDF_LIB_QUALITY_DEFAULT = 1,
    PDF_LIB_QUALITY_EBOOK = 2,
    PDF_LIB_QUALITY_PRINTER = 3,
    PDF_LIB_QUALITY_PREPRESS = 4
} pdf_lib_quality_t;

/**
 * @brief Paper size enumeration
 */
typedef enum {
    PDF_LIB_PAPER_CUSTOM = 0,
    PDF_LIB_PAPER_A4 = 1,
    PDF_LIB_PAPER_LETTER = 2,
    PDF_LIB_PAPER_LEGAL = 3,
    PDF_LIB_PAPER_A3 = 4,
    PDF_LIB_PAPER_A5 = 5,
    PDF_LIB_PAPER_EXECUTIVE = 6
} pdf_lib_paper_size_t;

/**
 * @brief Processing options structure
 */
typedef struct {
    int batch_mode;                    ///< Process files and exit
    int no_pause;                      ///< Disable pausing between pages
    int quiet;                         ///< Suppress output messages
    int safer_mode;                    ///< Enable safe mode operations
    int printed;                       ///< Mark output as printed
    int empty_stack;                   ///< Start with empty stack
    
    pdf_lib_quality_t quality;         ///< PDF quality setting
    double compatibility_level;        ///< PDF compatibility level
    int num_copies;                    ///< Number of copies for printing
    
    pdf_lib_paper_size_t paper_size;   ///< Paper size
    double custom_width_points;        ///< Custom width in points
    double custom_height_points;       ///< Custom height in points
    
    double left_margin;                ///< Left margin in points
    double right_margin;               ///< Right margin in points
    double top_margin;                 ///< Top margin in points
    double bottom_margin;              ///< Bottom margin in points
    
    double page_offset_x;              ///< Page offset X in points
    double page_offset_y;              ///< Page offset Y in points
    
    const char* output_file;           ///< Output file path
    const char* device_name;           ///< Device name
    const char* printer_name;          ///< Printer name for Windows printing
    
    const char** input_files;          ///< Input file paths array
    int input_files_count;             ///< Number of input files
    
    const char** postscript_commands;  ///< PostScript commands array
    int postscript_commands_count;     ///< Number of PostScript commands
} pdf_lib_options_t;

/**
 * @brief Error information structure
 */
typedef struct {
    pdf_lib_error_code_t code;         ///< Error code
    const char* message;               ///< Error message
    const char* file;                  ///< File where error occurred
    int line;                          ///< Line number where error occurred
} pdf_lib_error_info_t;

/**
 * @brief Progress callback function type
 */
typedef void (*pdf_lib_progress_callback_t)(int current, int total, const char* status, void* user_data);

/**
 * @brief PDF processor handle
 */
typedef struct pdf_lib_processor* pdf_lib_processor_t;

/**
 * @brief Create a new PDF processor instance
 * @return Processor handle or NULL on failure
 */
PDF_LIB_C_API pdf_lib_processor_t pdf_lib_create_processor(void);

/**
 * @brief Destroy a PDF processor instance
 * @param processor Processor handle
 */
PDF_LIB_C_API void pdf_lib_destroy_processor(pdf_lib_processor_t processor);

/**
 * @brief Initialize processor with command-line arguments
 * @param processor Processor handle
 * @param arguments Command-line arguments array
 * @param argument_count Number of arguments
 * @return Error code
 */
PDF_LIB_C_API pdf_lib_error_code_t pdf_lib_initialize_with_arguments(
    pdf_lib_processor_t processor,
    const char** arguments,
    int argument_count
);

/**
 * @brief Initialize processor with options structure
 * @param processor Processor handle
 * @param options Processing options
 * @return Error code
 */
PDF_LIB_C_API pdf_lib_error_code_t pdf_lib_initialize_with_options(
    pdf_lib_processor_t processor,
    const pdf_lib_options_t* options
);

/**
 * @brief Execute the processing operation
 * @param processor Processor handle
 * @return Error code
 */
PDF_LIB_C_API pdf_lib_error_code_t pdf_lib_process(pdf_lib_processor_t processor);

/**
 * @brief Set progress callback function
 * @param processor Processor handle
 * @param callback Progress callback function
 * @param user_data User data passed to callback
 */
PDF_LIB_C_API void pdf_lib_set_progress_callback(
    pdf_lib_processor_t processor,
    pdf_lib_progress_callback_t callback,
    void* user_data
);

/**
 * @brief Get the last error information
 * @param processor Processor handle
 * @param error_info Output error information structure
 */
PDF_LIB_C_API void pdf_lib_get_last_error(
    pdf_lib_processor_t processor,
    pdf_lib_error_info_t* error_info
);

/**
 * @brief Get error message as string
 * @param processor Processor handle
 * @return Error message string (caller must not free)
 */
PDF_LIB_C_API const char* pdf_lib_get_last_error_string(pdf_lib_processor_t processor);

/**
 * @brief Clean up resources
 * @param processor Processor handle
 */
PDF_LIB_C_API void pdf_lib_cleanup(pdf_lib_processor_t processor);

/**
 * @brief Check if processor is initialized
 * @param processor Processor handle
 * @return 1 if initialized, 0 otherwise
 */
PDF_LIB_C_API int pdf_lib_is_initialized(pdf_lib_processor_t processor);

/**
 * @brief Get current processing options
 * @param processor Processor handle
 * @param options Output options structure
 * @return Error code
 */
PDF_LIB_C_API pdf_lib_error_code_t pdf_lib_get_options(
    pdf_lib_processor_t processor,
    pdf_lib_options_t* options
);

/**
 * @brief Utility function to check if file exists
 * @param filepath File path to check
 * @return 1 if file exists, 0 otherwise
 */
PDF_LIB_C_API int pdf_lib_file_exists(const char* filepath);

/**
 * @brief Utility function to get file extension
 * @param filepath File path
 * @return File extension string (caller must not free)
 */
PDF_LIB_C_API const char* pdf_lib_get_file_extension(const char* filepath);

/**
 * @brief Utility function to create temporary file
 * @param prefix File prefix (can be NULL for default)
 * @param extension File extension (can be NULL for default)
 * @return Temporary file path (caller must free with pdf_lib_free_string)
 */
PDF_LIB_C_API char* pdf_lib_create_temp_file(const char* prefix, const char* extension);

/**
 * @brief Utility function to delete file
 * @param filepath File path to delete
 * @return 1 if deletion successful, 0 otherwise
 */
PDF_LIB_C_API int pdf_lib_delete_file(const char* filepath);

/**
 * @brief Free string allocated by the library
 * @param str String to free
 */
PDF_LIB_C_API void pdf_lib_free_string(char* str);

/**
 * @brief Get library version string
 * @return Version string (caller must not free)
 */
PDF_LIB_C_API const char* pdf_lib_get_version(void);

/**
 * @brief Get library build information
 * @return Build info string (caller must not free)
 */
PDF_LIB_C_API const char* pdf_lib_get_build_info(void);

#ifdef __cplusplus
}
#endif

#endif // PDF_LIB_C_H 