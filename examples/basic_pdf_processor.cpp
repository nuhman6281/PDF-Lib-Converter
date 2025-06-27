/**
 * @file basic_pdf_processor.cpp
 * @brief Basic PDF processing example
 * @version 1.0.0
 * @author PDF_LIB Team
 * 
 * This example demonstrates basic usage of the PDF_LIB library
 * for PDF processing operations.
 */

#include "../include/pdf_lib.h"
#include <iostream>
#include <vector>

using namespace PDFLib;

int main() {
    std::cout << "PDF_LIB Basic PDF Processor Example" << std::endl;
    std::cout << "===================================" << std::endl;
    
    // Create PDF processor instance
    PDFProcessor processor;
    
    // Set up processing options
    ProcessingOptions options;
    options.device_name = "pdfwrite";
    options.output_file = "output.pdf";
    options.input_files = {"input.ps"};  // You would need a real input file
    options.quality = PDFQuality::PRINTER;
    options.paper_size = PaperSize::A4;
    options.compatibility_level = 1.7;
    options.num_copies = 1;
    options.batch_mode = true;
    options.quiet = false;
    
    // Set progress callback
    processor.SetProgressCallback([](int current, int total, const std::string& status) {
        std::cout << "Progress: " << current << "/" << total << " - " << status << std::endl;
    });
    
    // Initialize the processor
    std::cout << "Initializing processor..." << std::endl;
    if (!processor.Initialize(options)) {
        std::cerr << "Failed to initialize processor: " << processor.GetLastErrorString() << std::endl;
        return 1;
    }
    
    std::cout << "Processor initialized successfully." << std::endl;
    
    // Process the document
    std::cout << "Processing document..." << std::endl;
    if (!processor.Process()) {
        std::cerr << "Failed to process document: " << processor.GetLastErrorString() << std::endl;
        return 1;
    }
    
    std::cout << "Document processed successfully!" << std::endl;
    
    // Get final options
    ProcessingOptions final_options = processor.GetOptions();
    std::cout << "Final options:" << std::endl;
    std::cout << "  Device: " << final_options.device_name << std::endl;
    std::cout << "  Output file: " << final_options.output_file << std::endl;
    std::cout << "  Quality: " << Utils::QualityToString(final_options.quality) << std::endl;
    std::cout << "  Paper size: " << Utils::PaperSizeToString(final_options.paper_size) << std::endl;
    
    // Clean up
    processor.Cleanup();
    std::cout << "Cleanup completed." << std::endl;
    
    return 0;
} 