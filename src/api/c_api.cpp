/**
 * @file c_api.cpp
 * @brief C API wrapper implementation for PDF_LIB
 */

#include "../include/pdf_lib_c.h"
#include "../core/pdf_processor.h"
#include "../utils/file_utils.h"
#include "../utils/string_utils.h"
#include <memory>
#include <vector>
#include <string>
#include <cstring>

namespace PDFLib {

// Internal processor structure
struct pdf_lib_processor {
    std::unique_ptr<PDFProcessor> processor;
    std::string last_error;
    pdf_lib_progress_callback_t progress_callback;
    void* user_data;
    
    pdf_lib_processor() : progress_callback(nullptr), user_data(nullptr) {
        processor = std::make_unique<PDFProcessor>();
    }
};

// Global error strings
static std::string g_last_error_string;
static std::string g_version_string = "1.0.0";
static std::string g_build_info_string = "PDF_LIB v1.0.0 - Ghostscript Alternative";

// Helper function to convert C++ string to C string
const char* StringToCString(const std::string& str) {
    g_last_error_string = str;
    return g_last_error_string.c_str();
}

// Helper function to convert C string to C++ string
std::string CStringToString(const char* str) {
    return str ? std::string(str) : std::string();
}

// Helper function to convert ProcessingOptions
ProcessingOptions ConvertToProcessingOptions(const pdf_lib_options_t* options) {
    ProcessingOptions cpp_options;
    
    if (!options) {
        return cpp_options;
    }
    
    cpp_options.batch_mode = options->batch_mode != 0;
    cpp_options.no_pause = options->no_pause != 0;
    cpp_options.quiet = options->quiet != 0;
    cpp_options.safer_mode = options->safer_mode != 0;
    cpp_options.printed = options->printed != 0;
    cpp_options.empty_stack = options->empty_stack != 0;
    
    switch (options->quality) {
        case PDF_LIB_QUALITY_SCREEN: cpp_options.quality = PDFQuality::SCREEN; break;
        case PDF_LIB_QUALITY_EBOOK: cpp_options.quality = PDFQuality::EBOOK; break;
        case PDF_LIB_QUALITY_PRINTER: cpp_options.quality = PDFQuality::PRINTER; break;
        case PDF_LIB_QUALITY_PREPRESS: cpp_options.quality = PDFQuality::PREPRESS; break;
        default: cpp_options.quality = PDFQuality::DEFAULT; break;
    }
    
    cpp_options.compatibility_level = options->compatibility_level;
    cpp_options.num_copies = options->num_copies;
    
    switch (options->paper_size) {
        case PDF_LIB_PAPER_CUSTOM: cpp_options.paper_size = PaperSize::CUSTOM; break;
        case PDF_LIB_PAPER_LETTER: cpp_options.paper_size = PaperSize::LETTER; break;
        case PDF_LIB_PAPER_LEGAL: cpp_options.paper_size = PaperSize::LEGAL; break;
        case PDF_LIB_PAPER_A3: cpp_options.paper_size = PaperSize::A3; break;
        case PDF_LIB_PAPER_A5: cpp_options.paper_size = PaperSize::A5; break;
        case PDF_LIB_PAPER_EXECUTIVE: cpp_options.paper_size = PaperSize::EXECUTIVE; break;
        default: cpp_options.paper_size = PaperSize::A4; break;
    }
    
    cpp_options.custom_width_points = options->custom_width_points;
    cpp_options.custom_height_points = options->custom_height_points;
    cpp_options.left_margin = options->left_margin;
    cpp_options.right_margin = options->right_margin;
    cpp_options.top_margin = options->top_margin;
    cpp_options.bottom_margin = options->bottom_margin;
    cpp_options.page_offset_x = options->page_offset_x;
    cpp_options.page_offset_y = options->page_offset_y;
    
    cpp_options.output_file = CStringToString(options->output_file);
    cpp_options.device_name = CStringToString(options->device_name);
    cpp_options.printer_name = CStringToString(options->printer_name);
    
    // Convert input files array
    if (options->input_files && options->input_files_count > 0) {
        for (int i = 0; i < options->input_files_count; ++i) {
            if (options->input_files[i]) {
                cpp_options.input_files.push_back(options->input_files[i]);
            }
        }
    }
    
    // Convert PostScript commands array
    if (options->postscript_commands && options->postscript_commands_count > 0) {
        for (int i = 0; i < options->postscript_commands_count; ++i) {
            if (options->postscript_commands[i]) {
                cpp_options.postscript_commands.push_back(options->postscript_commands[i]);
            }
        }
    }
    
    return cpp_options;
}

// Helper function to convert ProcessingOptions back to C structure
void ConvertFromProcessingOptions(const ProcessingOptions& cpp_options, pdf_lib_options_t* options) {
    if (!options) {
        return;
    }
    
    options->batch_mode = cpp_options.batch_mode ? 1 : 0;
    options->no_pause = cpp_options.no_pause ? 1 : 0;
    options->quiet = cpp_options.quiet ? 1 : 0;
    options->safer_mode = cpp_options.safer_mode ? 1 : 0;
    options->printed = cpp_options.printed ? 1 : 0;
    options->empty_stack = cpp_options.empty_stack ? 1 : 0;
    
    switch (cpp_options.quality) {
        case PDFQuality::SCREEN: options->quality = PDF_LIB_QUALITY_SCREEN; break;
        case PDFQuality::EBOOK: options->quality = PDF_LIB_QUALITY_EBOOK; break;
        case PDFQuality::PRINTER: options->quality = PDF_LIB_QUALITY_PRINTER; break;
        case PDFQuality::PREPRESS: options->quality = PDF_LIB_QUALITY_PREPRESS; break;
        default: options->quality = PDF_LIB_QUALITY_DEFAULT; break;
    }
    
    options->compatibility_level = cpp_options.compatibility_level;
    options->num_copies = cpp_options.num_copies;
    
    switch (cpp_options.paper_size) {
        case PaperSize::CUSTOM: options->paper_size = PDF_LIB_PAPER_CUSTOM; break;
        case PaperSize::LETTER: options->paper_size = PDF_LIB_PAPER_LETTER; break;
        case PaperSize::LEGAL: options->paper_size = PDF_LIB_PAPER_LEGAL; break;
        case PaperSize::A3: options->paper_size = PDF_LIB_PAPER_A3; break;
        case PaperSize::A5: options->paper_size = PDF_LIB_PAPER_A5; break;
        case PaperSize::EXECUTIVE: options->paper_size = PDF_LIB_PAPER_EXECUTIVE; break;
        default: options->paper_size = PDF_LIB_PAPER_A4; break;
    }
    
    options->custom_width_points = cpp_options.custom_width_points;
    options->custom_height_points = cpp_options.custom_height_points;
    options->left_margin = cpp_options.left_margin;
    options->right_margin = cpp_options.right_margin;
    options->top_margin = cpp_options.top_margin;
    options->bottom_margin = cpp_options.bottom_margin;
    options->page_offset_x = cpp_options.page_offset_x;
    options->page_offset_y = cpp_options.page_offset_y;
    
    // Note: We can't set the string pointers directly as they would become invalid
    // The caller needs to manage the string memory
}

// Helper function to convert error code
pdf_lib_error_code_t ConvertErrorCode(int cpp_error_code) {
    switch (cpp_error_code) {
        case 0: return PDF_LIB_SUCCESS;
        case -1: return PDF_LIB_ERROR_INVALID_ARGUMENT;
        case -2: return PDF_LIB_ERROR_FILE_NOT_FOUND;
        case -3: return PDF_LIB_ERROR_FILE_ACCESS_DENIED;
        case -4: return PDF_LIB_ERROR_INVALID_PDF;
        case -5: return PDF_LIB_ERROR_PRINTER_NOT_FOUND;
        case -6: return PDF_LIB_ERROR_PRINTER_ACCESS_DENIED;
        case -7: return PDF_LIB_ERROR_OUT_OF_MEMORY;
        case -8: return PDF_LIB_ERROR_INTERNAL;
        case -9: return PDF_LIB_ERROR_NOT_INITIALIZED;
        case -10: return PDF_LIB_ERROR_ALREADY_INITIALIZED;
        case -11: return PDF_LIB_ERROR_UNSUPPORTED_FORMAT;
        case -12: return PDF_LIB_ERROR_POSTSCRIPT_ERROR;
        default: return PDF_LIB_ERROR_INTERNAL;
    }
}

} // namespace PDFLib

// C API implementation
extern "C" {

pdf_lib_processor_t pdf_lib_create_processor(void) {
    try {
        return new PDFLib::pdf_lib_processor();
    } catch (...) {
        return nullptr;
    }
}

void pdf_lib_destroy_processor(pdf_lib_processor_t processor) {
    if (processor) {
        delete processor;
    }
}

pdf_lib_error_code_t pdf_lib_initialize_with_arguments(
    pdf_lib_processor_t processor,
    const char** arguments,
    int argument_count) {
    
    if (!processor || !arguments || argument_count <= 0) {
        return PDF_LIB_ERROR_INVALID_ARGUMENT;
    }
    
    try {
        std::vector<std::string> args;
        for (int i = 0; i < argument_count; ++i) {
            if (arguments[i]) {
                args.push_back(arguments[i]);
            }
        }
        
        if (!processor->processor->Initialize(args)) {
            processor->last_error = processor->processor->GetLastErrorString();
            return ConvertErrorCode(processor->processor->GetLastError().code);
        }
        
        return PDF_LIB_SUCCESS;
    } catch (...) {
        return PDF_LIB_ERROR_INTERNAL;
    }
}

pdf_lib_error_code_t pdf_lib_initialize_with_options(
    pdf_lib_processor_t processor,
    const pdf_lib_options_t* options) {
    
    if (!processor || !options) {
        return PDF_LIB_ERROR_INVALID_ARGUMENT;
    }
    
    try {
        PDFLib::ProcessingOptions cpp_options = PDFLib::ConvertToProcessingOptions(options);
        
        if (!processor->processor->Initialize(cpp_options)) {
            processor->last_error = processor->processor->GetLastErrorString();
            return ConvertErrorCode(processor->processor->GetLastError().code);
        }
        
        return PDF_LIB_SUCCESS;
    } catch (...) {
        return PDF_LIB_ERROR_INTERNAL;
    }
}

pdf_lib_error_code_t pdf_lib_process(pdf_lib_processor_t processor) {
    if (!processor) {
        return PDF_LIB_ERROR_INVALID_ARGUMENT;
    }
    
    try {
        if (!processor->processor->Process()) {
            processor->last_error = processor->processor->GetLastErrorString();
            return ConvertErrorCode(processor->processor->GetLastError().code);
        }
        
        return PDF_LIB_SUCCESS;
    } catch (...) {
        return PDF_LIB_ERROR_INTERNAL;
    }
}

void pdf_lib_set_progress_callback(
    pdf_lib_processor_t processor,
    pdf_lib_progress_callback_t callback,
    void* user_data) {
    
    if (!processor) {
        return;
    }
    
    processor->progress_callback = callback;
    processor->user_data = user_data;
    
    if (callback) {
        processor->processor->SetProgressCallback([processor](int current, int total, const std::string& status) {
            if (processor->progress_callback) {
                processor->progress_callback(current, total, status.c_str(), processor->user_data);
            }
        });
    }
}

void pdf_lib_get_last_error(
    pdf_lib_processor_t processor,
    pdf_lib_error_info_t* error_info) {
    
    if (!processor || !error_info) {
        return;
    }
    
    PDFLib::ErrorInfo cpp_error = processor->processor->GetLastError();
    error_info->code = ConvertErrorCode(cpp_error.code);
    error_info->message = PDFLib::StringToCString(cpp_error.message);
    error_info->file = PDFLib::StringToCString(cpp_error.file);
    error_info->line = cpp_error.line;
}

const char* pdf_lib_get_last_error_string(pdf_lib_processor_t processor) {
    if (!processor) {
        return PDFLib::StringToCString("Invalid processor");
    }
    
    return PDFLib::StringToCString(processor->last_error);
}

void pdf_lib_cleanup(pdf_lib_processor_t processor) {
    if (processor) {
        processor->processor->Cleanup();
    }
}

int pdf_lib_is_initialized(pdf_lib_processor_t processor) {
    if (!processor) {
        return 0;
    }
    
    return processor->processor->IsInitialized() ? 1 : 0;
}

pdf_lib_error_code_t pdf_lib_get_options(
    pdf_lib_processor_t processor,
    pdf_lib_options_t* options) {
    
    if (!processor || !options) {
        return PDF_LIB_ERROR_INVALID_ARGUMENT;
    }
    
    try {
        PDFLib::ProcessingOptions cpp_options = processor->processor->GetOptions();
        PDFLib::ConvertFromProcessingOptions(cpp_options, options);
        return PDF_LIB_SUCCESS;
    } catch (...) {
        return PDF_LIB_ERROR_INTERNAL;
    }
}

int pdf_lib_file_exists(const char* filepath) {
    if (!filepath) {
        return 0;
    }
    
    return PDFLib::Utils::FileExists(filepath) ? 1 : 0;
}

const char* pdf_lib_get_file_extension(const char* filepath) {
    if (!filepath) {
        return PDFLib::StringToCString("");
    }
    
    return PDFLib::StringToCString(PDFLib::Utils::GetFileExtension(filepath));
}

char* pdf_lib_create_temp_file(const char* prefix, const char* extension) {
    std::string cpp_prefix = PDFLib::CStringToString(prefix);
    std::string cpp_extension = PDFLib::CStringToString(extension);
    
    std::string temp_file = PDFLib::Utils::CreateTempFile(cpp_prefix, cpp_extension);
    
    if (temp_file.empty()) {
        return nullptr;
    }
    
    char* result = static_cast<char*>(malloc(temp_file.length() + 1));
    if (result) {
        strcpy(result, temp_file.c_str());
    }
    
    return result;
}

int pdf_lib_delete_file(const char* filepath) {
    if (!filepath) {
        return 0;
    }
    
    return PDFLib::Utils::DeleteFile(filepath) ? 1 : 0;
}

void pdf_lib_free_string(char* str) {
    if (str) {
        free(str);
    }
}

const char* pdf_lib_get_version(void) {
    return PDFLib::g_version_string.c_str();
}

const char* pdf_lib_get_build_info(void) {
    return PDFLib::g_build_info_string.c_str();
}

} // extern "C" 