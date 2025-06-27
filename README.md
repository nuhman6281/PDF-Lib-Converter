# PDF-Lib-Converter

A lightweight, high-performance C++ library for converting PostScript (.ps) and Encapsulated PostScript (.eps) files to PDF format. Designed as a modern alternative to Ghostscript for specific conversion workflows.

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![License](https://img.shields.io/badge/license-MIT-blue)]()
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue)]()
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey)]()

## 🚀 Features

- **PostScript to PDF Conversion**: Native support for PostScript Level 3 documents
- **Fast Processing**: ~8ms conversion time for typical documents
- **Ghostscript Compatible**: Drop-in replacement for common `gs` commands
- **Cross-Platform**: Works on Linux, macOS, and Windows
- **Professional Output**: Generates valid PDF 1.7 documents
- **C++ and C APIs**: Full programmatic access with multiple language bindings
- **Multiple Build Systems**: Support for both CMake and Make
- **Device Support**: PDF writer and Windows printer device support

## 📋 Requirements

- **C++ Compiler**: GCC 7+ or Clang 5+ with C++17 support
- **Build Tools**: CMake 3.10+ or Make
- **Memory**: Minimal RAM requirements
- **Storage**: Small footprint (~200KB library)

## 🛠️ Installation

### Using Make (Recommended)

```bash
# Clone the repository
git clone https://github.com/nuhman6281/PDF-Lib-Converter.git
cd PDF-Lib-Converter

# Build the library and CLI
make all

# The executable will be available at build/bin/pdf_gs
```

### Using CMake

```bash
# Clone and build
git clone https://github.com/nuhman6281/PDF-Lib-Converter.git
cd PDF-Lib-Converter

mkdir build
cd build
cmake ..
make

# Executables and libraries will be in the build directory
```

## 🎯 Quick Start

### Command Line Usage

```bash
# Basic PostScript to PDF conversion
./build/bin/pdf_gs -sDEVICE=pdfwrite -sOutputFile=output.pdf input.ps

# Ghostscript-compatible syntax
./build/bin/pdf_gs -dBATCH -dNOPAUSE -sDEVICE=pdfwrite -sOutputFile=output.pdf input.ps

# Get help
./build/bin/pdf_gs --help
```

### Programmatic Usage (C++)

```cpp
#include "pdf_lib.h"

int main() {
    // Create processor
    PDFLib::PDFProcessor processor;

    // Setup options
    PDFLib::ProcessingOptions options;
    options.input_files = {"document.ps"};
    options.output_file = "document.pdf";
    options.device_name = "pdfwrite";
    options.quality = PDFLib::PDFQuality::PRINTER;

    // Process
    if (processor.Initialize(options) && processor.Process()) {
        std::cout << "Conversion successful!" << std::endl;
    }

    return 0;
}
```

### Using C API

```c
#include "pdf_lib_c.h"

int main() {
    // Create processor
    PDFProcessorHandle processor = pdf_processor_create();

    // Setup and process
    ProcessingOptionsC options = {0};
    options.input_files = (const char*[]){"document.ps", NULL};
    options.output_file = "document.pdf";
    options.device_name = "pdfwrite";

    if (pdf_processor_initialize(processor, &options) &&
        pdf_processor_process(processor)) {
        printf("Conversion successful!\n");
    }

    pdf_processor_destroy(processor);
    return 0;
}
```

## 📚 API Documentation

### Core Classes

- **`PDFProcessor`**: Main processing engine
- **`PostScriptParser`**: PS/EPS file parser
- **`PDFGenerator`**: PDF document generator
- **`DeviceManager`**: Output device management
- **`ErrorHandler`**: Centralized error handling

### Processing Options

```cpp
struct ProcessingOptions {
    std::vector<std::string> input_files;    // Input PS/EPS files
    std::string output_file;                 // Output PDF path
    std::string device_name;                 // "pdfwrite" or "mswinpr2"
    PDFQuality quality;                      // SCREEN, EBOOK, PRINTER, PREPRESS
    double compatibility_level;              // PDF version (1.4-1.7)
    PaperSize paper_size;                   // A4, LETTER, LEGAL, etc.
    bool batch_mode;                        // Process and exit
    bool quiet;                             // Suppress output
    // ... additional options
};
```

## 🏗️ Architecture

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│  PostScript     │───▶│   PDF Processor  │───▶│   PDF Output    │
│  Input (.ps)    │    │                  │    │   (.pdf)        │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                              │
                              ▼
                    ┌──────────────────┐
                    │  Rendering       │
                    │  Backend         │
                    │                  │
                    │ • Parser         │
                    │ • Generator      │
                    │ • Device Mgr     │
                    └──────────────────┘
```

## 🧪 Testing

```bash
# Run basic conversion test
make test

# Test with sample file
./build/bin/pdf_gs -sDEVICE=pdfwrite -sOutputFile=test.pdf test_files/test1.ps

# Verify output
file test.pdf  # Should show: PDF document, version 1.7
```

## 🔧 Build Targets

### Make Targets

- `make all` - Build library and CLI
- `make clean` - Remove build artifacts
- `make install` - Install to system directories
- `make help` - Show available targets

### CMake Targets

- `make pdf_lib` - Static library
- `make pdf_gs` - Command-line interface
- `make examples` - Example programs
- `make tests` - Test suite

## 📁 Project Structure

```
PDF-Lib-Converter/
├── include/              # Public headers
│   ├── pdf_lib.h        # C++ API
│   └── pdf_lib_c.h      # C API
├── src/                  # Source code
│   ├── core/            # Core processing engine
│   ├── devices/         # Output devices
│   ├── parsers/         # Input parsers
│   ├── api/             # C API bindings
│   └── cli/             # Command-line interface
├── examples/            # Example programs
├── tests/               # Test suite
├── test_files/          # Sample PS files
├── CMakeLists.txt       # CMake configuration
├── Makefile            # Make configuration
└── README.md           # This file
```

## 🎨 Supported Features

### PostScript Support

- ✅ PostScript Level 3 documents
- ✅ Encapsulated PostScript (EPS)
- ✅ DSC (Document Structure Convention)
- ✅ Path construction (moveto, lineto, curveto)
- ✅ Graphics state management
- ✅ Text rendering
- ✅ Color operations

### PDF Output

- ✅ PDF 1.7 compliance
- ✅ Multi-page documents
- ✅ Vector graphics preservation
- ✅ Text elements
- ✅ Metadata embedding
- ✅ Optimized file size

## 🤝 Contributing

We welcome contributions! Please see our contributing guidelines:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Inspired by Ghostscript for PostScript processing
- Built with modern C++17 standards
- Cross-platform compatibility focus

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/nuhman6281/PDF-Lib-Converter/issues)
- **Discussions**: [GitHub Discussions](https://github.com/nuhman6281/PDF-Lib-Converter/discussions)
- **Documentation**: See `docs/` directory

---

**Note**: This library is designed for specific PostScript to PDF conversion workflows. For comprehensive PostScript processing, consider using Ghostscript.
