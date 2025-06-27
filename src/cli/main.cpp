/**
 * @file main.cpp
 * @brief Command-line interface for PDF_LIB
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include "../include/pdf_lib.h"

void PrintUsage() {
    std::cout << "PDF_LIB - A lightweight Ghostscript alternative\n\n";
    std::cout << "Usage: pdf_gs [options] [files]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -dBATCH              Process files and exit\n";
    std::cout << "  -dNOPAUSE            Disable pausing between pages\n";
    std::cout << "  -dQUIET              Suppress output messages\n";
    std::cout << "  -dSAFER              Enable safe mode operations\n";
    std::cout << "  -dNOSAFER            Disable safe mode\n";
    std::cout << "  -dPrinted            Mark output as printed\n";
    std::cout << "  -empty               Start with empty stack\n";
    std::cout << "  -dAutoRotatePages=<value>  Control page rotation (None/All/PageByPage)\n";
    std::cout << "  -dPDFSETTINGS=<value>       PDF quality settings (screen/default/ebook/printer/prepress)\n";
    std::cout << "  -dCompatibilityLevel=<value> PDF version compatibility\n";
    std::cout << "  -dNumCopies=<value>         Number of copies for printing\n";
    std::cout << "  -sDEVICE=<devicename>       Specify output device (pdfwrite/mswinpr2)\n";
    std::cout << "  -sOutputFile=<path>         Output file path\n";
    std::cout << "  -sPAPERSIZE=<size>          Paper size specification (custom/a4/letter/legal/a3/a5/executive)\n";
    std::cout << "  -dDEVICEWIDTHPOINTS=<value> Custom width in points\n";
    std::cout << "  -dDEVICEHEIGHTPOINTS=<value> Custom height in points\n";
    std::cout << "  -dPDFX11LeftMargin=<value>  Left margin control\n";
    std::cout << "  -c \"<postscript_commands>\" Execute PostScript commands\n";
    std::cout << "  -f                     Process files after commands\n";
    std::cout << "  -h, --help            Show this help message\n";
    std::cout << "  -v, --version         Show version information\n\n";
    std::cout << "Examples:\n";
    std::cout << "  pdf_gs -sDEVICE=pdfwrite -sOutputFile=output.pdf input.pdf\n";
    std::cout << "  pdf_gs -sDEVICE=mswinpr2 -sOutputFile=\"%printer%PrinterName\" input.pdf\n";
    std::cout << "  pdf_gs -dPDFSETTINGS=printer -sOutputFile=optimized.pdf input.pdf\n";
    std::cout << "  pdf_gs -c \"<< /PageOffset [10 20] >> setpagedevice\" -f input.pdf\n\n";
}

void PrintVersion() {
    std::cout << "PDF_LIB version 1.0.0\n";
    std::cout << "A lightweight Ghostscript alternative for PDF processing and printing\n";
}

void PrintError(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
}

void PrintWarning(const std::string& message) {
    std::cout << "Warning: " << message << std::endl;
}

void PrintInfo(const std::string& message) {
    std::cout << "Info: " << message << std::endl;
}

void ProgressCallback(int current, int total, const std::string& status) {
    if (total > 0) {
        int percentage = (current * 100) / total;
        std::cout << "\rProgress: " << percentage << "% - " << status;
        if (current >= total) {
            std::cout << std::endl;
        }
        std::cout.flush();
    } else {
        std::cout << "\r" << status;
        std::cout.flush();
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        PrintUsage();
        return 1;
    }
    
    // Parse command-line arguments
    std::vector<std::string> arguments;
    for (int i = 1; i < argc; ++i) {
        arguments.push_back(argv[i]);
    }
    
    // Check for help and version options
    for (const auto& arg : arguments) {
        if (arg == "-h" || arg == "--help") {
            PrintUsage();
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            PrintVersion();
            return 0;
        }
    }
    
    try {
        // Create PDF processor
        PDFLib::PDFProcessor processor;
        
        // Set progress callback
        processor.SetProgressCallback(ProgressCallback);
        
        // Initialize processor with arguments
        if (!processor.Initialize(arguments)) {
            PrintError(processor.GetLastErrorString());
            return 1;
        }
        
        // Get processing options for validation
        PDFLib::ProcessingOptions options = processor.GetOptions();
        
        // Validate options
        if (options.input_files.empty() && options.device_name != "mswinpr2") {
            PrintError("No input files specified");
            return 1;
        }
        
        if (options.output_file.empty() && options.device_name != "mswinpr2") {
            PrintError("No output file specified");
            return 1;
        }
        
        // Print processing information
        if (!options.quiet) {
            PrintInfo("Starting PDF processing...");
            if (!options.input_files.empty()) {
                PrintInfo("Input files: " + std::to_string(options.input_files.size()));
                for (const auto& file : options.input_files) {
                    PrintInfo("  - " + file);
                }
            }
            if (!options.output_file.empty()) {
                PrintInfo("Output file: " + options.output_file);
            }
            if (!options.device_name.empty()) {
                PrintInfo("Device: " + options.device_name);
            }
            if (!options.printer_name.empty()) {
                PrintInfo("Printer: " + options.printer_name);
            }
        }
        
        // Process files
        if (!processor.Process()) {
            PrintError(processor.GetLastErrorString());
            return 1;
        }
        
        if (!options.quiet) {
            PrintInfo("PDF processing completed successfully");
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        PrintError("Exception: " + std::string(e.what()));
        return 1;
    } catch (...) {
        PrintError("Unknown error occurred");
        return 1;
    }
} 