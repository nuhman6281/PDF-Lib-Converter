#include "include/pdf_lib.h"
#include <iostream>
#include <vector>
#include <string>

int main() {
    std::cout << "PDF_LIB PostScript to PDF Converter" << std::endl;
    std::cout << "====================================" << std::endl;
    
    try {
        // Create processor instance
        PDFLib::PDFProcessor processor;
        
        // Simulate Ghostscript command line:
        // gs -dBATCH -dNOPAUSE -sDEVICE=pdfwrite -sOutputFile=test1_output.pdf test_files/test1.ps
        std::vector<std::string> args = {
            "-dBATCH",           // Process files and exit
            "-dNOPAUSE",         // Don't pause between pages
            "-sDEVICE=pdfwrite", // Use PDF output device
            "-dPDFSETTINGS=printer", // High quality PDF
            "-dCompatibilityLevel=1.7", // PDF 1.7 compatibility
            "-sOutputFile=test1_output.pdf", // Output file
            "test_files/test1.ps" // Input PostScript file
        };
        
        std::cout << "Command line equivalent:" << std::endl;
        std::cout << "gs ";
        for (const auto& arg : args) {
            std::cout << arg << " ";
        }
        std::cout << std::endl << std::endl;
        
        // Initialize processor with command-line arguments
        std::cout << "Initializing PDF processor with arguments..." << std::endl;
        if (!processor.Initialize(args)) {
            std::cerr << "❌ Failed to initialize processor: " << processor.GetLastErrorString() << std::endl;
            return 1;
        }
        std::cout << "✅ Processor initialized successfully" << std::endl;
        
        // Set up progress callback to show conversion progress
        processor.SetProgressCallback([](int current, int total, const std::string& status) {
            if (total > 0) {
                int percentage = (current * 100) / total;
                std::cout << "📊 Progress: " << percentage << "% - " << status << std::endl;
            } else {
                std::cout << "📋 Status: " << status << std::endl;
            }
        });
        
        // Perform the conversion
        std::cout << "\n🔄 Converting PostScript to PDF..." << std::endl;
        std::cout << "Input:  test_files/test1.ps" << std::endl;
        std::cout << "Output: test1_output.pdf" << std::endl;
        
        if (!processor.Process()) {
            std::cerr << "❌ Conversion failed: " << processor.GetLastErrorString() << std::endl;
            return 1;
        }
        
        std::cout << "✅ Conversion completed successfully!" << std::endl;
        
        // Check if output file was created
        if (PDFLib::Utils::FileExists("test1_output.pdf")) {
            std::cout << "✅ Output PDF file created successfully" << std::endl;
            std::cout << "📄 File: test1_output.pdf" << std::endl;
        } else {
            std::cout << "⚠️  Note: This is a skeleton implementation" << std::endl;
            std::cout << "   The actual PDF file creation requires PostScript parsing backend" << std::endl;
            std::cout << "   But the processing pipeline executed successfully!" << std::endl;
        }
        
        // Show some information about the processing
        auto options = processor.GetOptions();
        std::cout << "\n📋 Processing Details:" << std::endl;
        std::cout << "   Device: " << options.device_name << std::endl;
        std::cout << "   Quality: " << PDFLib::Utils::QualityToString(options.quality) << std::endl;
        std::cout << "   Paper size: " << PDFLib::Utils::PaperSizeToString(options.paper_size) << std::endl;
        std::cout << "   Batch mode: " << (options.batch_mode ? "Yes" : "No") << std::endl;
        std::cout << "   PDF compatibility: " << options.compatibility_level << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception occurred: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 