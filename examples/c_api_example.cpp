/**
 * @file c_api_example.cpp
 * @brief C API usage example
 * @version 1.0.0
 * @author PDF_LIB Team
 * 
 * This example demonstrates how to use the C API wrapper
 * for integration with other languages like C# via P/Invoke.
 */

#include "../include/pdf_lib_c.h"
#include <iostream>
#include <vector>

// Progress callback function
void progress_callback(int current, int total, const char* status, void* user_data) {
    std::cout << "Progress: " << current << "/" << total << " - " << status << std::endl;
}

int main() {
    std::cout << "PDF_LIB C API Example" << std::endl;
    std::cout << "=====================" << std::endl;
    
    // Create processor instance
    pdf_lib_processor_t processor = pdf_lib_create_processor();
    if (!processor) {
        std::cerr << "Failed to create processor" << std::endl;
        return 1;
    }
    
    std::cout << "Processor created successfully." << std::endl;
    
    // Set up options
    pdf_lib_options_t options;
    memset(&options, 0, sizeof(options));
    
    options.device_name = "pdfwrite";
    options.output_file = "output.pdf";
    options.quality = PDF_LIB_QUALITY_PRINTER;
    options.paper_size = PDF_LIB_PAPER_A4;
    options.compatibility_level = 1.7;
    options.num_copies = 1;
    options.batch_mode = 1;
    options.quiet = 0;
    
    // Set input files
    const char* input_files[] = {"input.ps"};
    options.input_files = input_files;
    options.input_files_count = 1;
    
    // Set progress callback
    pdf_lib_set_progress_callback(processor, progress_callback, nullptr);
    
    // Initialize processor
    std::cout << "Initializing processor..." << std::endl;
    pdf_lib_error_code_t result = pdf_lib_initialize_with_options(processor, &options);
    if (result != PDF_LIB_SUCCESS) {
        std::cerr << "Failed to initialize processor: " << result << std::endl;
        
        pdf_lib_error_info_t error_info;
        pdf_lib_get_last_error(processor, &error_info);
        std::cerr << "Error: " << error_info.message << std::endl;
        
        pdf_lib_destroy_processor(processor);
        return 1;
    }
    
    std::cout << "Processor initialized successfully." << std::endl;
    
    // Check if initialized
    if (!pdf_lib_is_initialized(processor)) {
        std::cerr << "Processor is not initialized" << std::endl;
        pdf_lib_destroy_processor(processor);
        return 1;
    }
    
    // Process document
    std::cout << "Processing document..." << std::endl;
    result = pdf_lib_process(processor);
    if (result != PDF_LIB_SUCCESS) {
        std::cerr << "Failed to process document: " << result << std::endl;
        
        pdf_lib_error_info_t error_info;
        pdf_lib_get_last_error(processor, &error_info);
        std::cerr << "Error: " << error_info.message << std::endl;
        
        pdf_lib_destroy_processor(processor);
        return 1;
    }
    
    std::cout << "Document processed successfully!" << std::endl;
    
    // Get final options
    pdf_lib_options_t final_options;
    memset(&final_options, 0, sizeof(final_options));
    
    if (pdf_lib_get_options(processor, &final_options) == PDF_LIB_SUCCESS) {
        std::cout << "Final options:" << std::endl;
        std::cout << "  Device: " << final_options.device_name << std::endl;
        std::cout << "  Output file: " << final_options.output_file << std::endl;
        std::cout << "  Quality: " << final_options.quality << std::endl;
        std::cout << "  Paper size: " << final_options.paper_size << std::endl;
    }
    
    // Clean up
    pdf_lib_cleanup(processor);
    pdf_lib_destroy_processor(processor);
    
    std::cout << "Cleanup completed." << std::endl;
    
    // Demonstrate utility functions
    std::cout << "\nUtility function examples:" << std::endl;
    
    const char* test_file = "test.txt";
    if (pdf_lib_file_exists(test_file)) {
        std::cout << "File exists: " << test_file << std::endl;
    } else {
        std::cout << "File does not exist: " << test_file << std::endl;
    }
    
    const char* extension = pdf_lib_get_file_extension("document.pdf");
    std::cout << "File extension: " << extension << std::endl;
    
    char* temp_file = pdf_lib_create_temp_file("test_", ".tmp");
    if (temp_file) {
        std::cout << "Created temp file: " << temp_file << std::endl;
        pdf_lib_free_string(temp_file);
    }
    
    return 0;
} 