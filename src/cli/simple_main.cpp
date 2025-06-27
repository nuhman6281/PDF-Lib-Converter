/**
 * @file simple_main.cpp
 * @brief Simplified command-line interface for PDF_LIB
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include "../include/pdf_lib.h"

void PrintUsage() {
    std::cout << "PDF_LIB - A lightweight Ghostscript alternative\n\n";
    std::cout << "Usage: pdf_gs -sDEVICE=pdfwrite -sOutputFile=<output.pdf> <input.ps>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -sDEVICE=pdfwrite     Output device (only pdfwrite supported)\n";
    std::cout << "  -sOutputFile=<path>   Output PDF file path\n";
    std::cout << "  -dBATCH               Process files and exit (ignored for compatibility)\n";
    std::cout << "  -dNOPAUSE             Disable pausing (ignored for compatibility)\n";
    std::cout << "  -h, --help            Show this help message\n";
    std::cout << "  -v, --version         Show version information\n\n";
    std::cout << "Example:\n";
    std::cout << "  pdf_gs -sDEVICE=pdfwrite -sOutputFile=output.pdf input.ps\n\n";
}

void PrintVersion() {
    std::cout << "PDF_LIB version 1.0.0\n";
    std::cout << "A lightweight Ghostscript alternative for PDF processing\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        PrintUsage();
        return 1;
    }
    
    std::string device;
    std::string output_file;
    std::vector<std::string> input_files;
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            PrintUsage();
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            PrintVersion();
            return 0;
        } else if (arg.substr(0, 9) == "-sDEVICE=") {
            device = arg.substr(9);
        } else if (arg.substr(0, 13) == "-sOutputFile=") {
            output_file = arg.substr(13);
        } else if (arg == "-dBATCH" || arg == "-dNOPAUSE" || arg == "-dQUIET" || arg == "-dSAFER") {
            // Ignore these flags for Ghostscript compatibility
            continue;
        } else if (arg[0] != '-') {
            // Input file
            input_files.push_back(arg);
        }
        // Ignore other flags
    }
    
    // Validate arguments
    if (device.empty()) {
        std::cerr << "Error: No device specified. Use -sDEVICE=pdfwrite\n";
        return 1;
    }
    
    if (device != "pdfwrite") {
        std::cerr << "Error: Only pdfwrite device is supported\n";
        return 1;
    }
    
    if (output_file.empty()) {
        std::cerr << "Error: No output file specified. Use -sOutputFile=output.pdf\n";
        return 1;
    }
    
    if (input_files.empty()) {
        std::cerr << "Error: No input files specified\n";
        return 1;
    }
    
    try {
        std::cout << "PDF_LIB PostScript to PDF Converter\n";
        std::cout << "Processing: " << input_files[0] << " -> " << output_file << "\n";
        
        // Create processor and setup options
        PDFLib::PDFProcessor processor;
        
        PDFLib::ProcessingOptions options;
        options.input_files = input_files;
        options.output_file = output_file;
        options.device_name = "pdfwrite";
        options.quality = PDFLib::PDFQuality::PRINTER;
        options.compatibility_level = 1.7;
        options.paper_size = PDFLib::PaperSize::A4;
        options.quiet = false;
        options.safer_mode = true;
        options.batch_mode = true;
        options.no_pause = true;
        
        // Initialize processor
        if (!processor.Initialize(options)) {
            std::cerr << "Error: " << processor.GetLastErrorString() << "\n";
            return 1;
        }
        
        // Process files
        if (!processor.Process()) {
            std::cerr << "Error: " << processor.GetLastErrorString() << "\n";
            return 1;
        }
        
        std::cout << "Conversion completed successfully\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Error: Unknown error occurred\n";
        return 1;
    }
} 