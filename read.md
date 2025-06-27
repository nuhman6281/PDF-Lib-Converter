# PDF_LIB - A Lightweight Ghostscript Alternative

PDF_LIB is a comprehensive native C++ library that serves as a lightweight alternative to Ghostscript, focusing on PDF processing and printing operations. It provides core Ghostscript functionality with a modern, cross-platform design and easy integration capabilities.

## Features

### Core Functionality

- **PDF Processing**: Create, manipulate, and convert PDF files
- **PostScript Support**: Process PostScript and EPS files
- **Printing Operations**: Direct printing to Windows printers
- **Device Support**: Multiple output devices (pdfwrite, mswinpr2, etc.)
- **Command-line Interface**: Ghostscript-compatible CLI

### Key Components

- **PDF Engine**: Core PDF processing engine
- **PostScript Interpreter**: PostScript language interpreter
- **Device Manager**: Manages output devices (PDF, printer, etc.)
- **Resource Manager**: Handles fonts, images, and other resources
- **Error Handler**: Comprehensive error handling and reporting
- **Command Parser**: Ghostscript-style command-line argument parsing

### Supported Devices

- `pdfwrite` - PDF output device
- `mswinpr2` - Windows printer device
- `ps2write` - PostScript Level 2 output
- `eps2write` - EPS Level 2 output
- `png16m`, `png256`, `pnggray` - PNG output devices
- `jpeg`, `jpeggray` - JPEG output devices
- `tiff*` - Various TIFF output devices
- And many more...

### Platform Support

- **Windows**: Full support with Windows printing APIs
- **Linux**: Core functionality (printing via CUPS)
- **macOS**: Core functionality (printing via CUPS)
- **Cross-platform**: C++ API with C wrapper for language integration

## Project Structure

```
PDF_LIB/
├── include/                 # Public headers
│   ├── pdf_lib.h           # Main C++ API
│   └── pdf_lib_c.h         # C API wrapper
├── src/                    # Source code
│   ├── core/              # Core components
│   ├── devices/           # Device implementations
│   ├── parsers/           # Command-line and PostScript parsers
│   ├── utils/             # Utility functions
│   ├── platform/          # Platform-specific code
│   ├── api/               # C API implementation
│   └── cli/               # Command-line interface
├── tests/                 # Unit tests
├── examples/              # Usage examples
├── CMakeLists.txt         # Main build configuration
└── README.md             # This file
```

## Building

### Prerequisites

- CMake 3.16 or higher
- C++17 compatible compiler
- Windows: Visual Studio 2019 or later
- Linux/macOS: GCC 7+ or Clang 5+

### Build Instructions

1. **Clone the repository**

   ```bash
   git clone <repository-url>
   cd PDF_LIB
   ```

2. **Create build directory**

   ```bash
   mkdir build
   cd build
   ```

3. **Configure with CMake**

   ```bash
   cmake ..
   ```

4. **Build the project**
   ```bash
   cmake --build .
   ```

### Build Options

- `BUILD_SHARED_LIBS=ON/OFF` - Build shared libraries (default: ON)
- `BUILD_TESTS=ON/OFF` - Build test suite (default: ON)
- `BUILD_EXAMPLES=ON/OFF` - Build examples (default: ON)
- `ENABLE_WINDOWS_PRINTER=ON/OFF` - Enable Windows printer support (default: ON)

### Optional Dependencies

- **PDFium**: For enhanced PDF processing (auto-detected)
- **GTest**: For unit testing (optional)

## Usage

### C++ API

```cpp
#include <pdf_lib.h>

using namespace PDFLib;

int main() {
    // Create processor
    PDFProcessor processor;

    // Set up options
    ProcessingOptions options;
    options.device_name = "pdfwrite";
    options.output_file = "output.pdf";
    options.input_files = {"input.ps"};
    options.quality = PDFQuality::PRINTER;
    options.paper_size = PaperSize::A4;

    // Initialize and process
    if (processor.Initialize(options)) {
        processor.Process();
    }

    return 0;
}
```

### C API (for language integration)

```c
#include <pdf_lib_c.h>

int main() {
    // Create processor
    pdf_lib_processor_t processor = pdf_lib_create_processor();

    // Set up options
    pdf_lib_options_t options = {0};
    options.device_name = "pdfwrite";
    options.output_file = "output.pdf";

    // Initialize and process
    if (pdf_lib_initialize_with_options(processor, &options) == PDF_LIB_SUCCESS) {
        pdf_lib_process(processor);
    }

    pdf_lib_destroy_processor(processor);
    return 0;
}
```

### Command-line Interface

```bash
# Convert PostScript to PDF
pdf_gs -sDEVICE=pdfwrite -sOutputFile=output.pdf input.ps

# Print PDF to Windows printer
pdf_gs -sDEVICE=mswinpr2 -sPrinterName="HP LaserJet" input.pdf

# Convert to PNG with specific resolution
pdf_gs -sDEVICE=png16m -r300 -g2480x3508 input.pdf

# Set quality and paper size
pdf_gs -sDEVICE=pdfwrite -dQuality=printer -sPaperSize=a4 input.ps
```

## Examples

The `examples/` directory contains several usage examples:

- `basic_pdf_processor.cpp` - Basic PDF processing
- `cli_example.cpp` - Command-line interface usage
- `c_api_example.cpp` - C API usage for language integration
- `windows_printing_example.cpp` - Windows printing (Windows only)
- `postscript_example.cpp` - PostScript processing

## Testing

Run the test suite:

```bash
cd build
ctest --verbose
```

Or run tests directly:

```bash
./bin/pdf_lib_tests
```

## API Documentation

### Main Classes

- **PDFProcessor**: Main processing class
- **Device**: Base class for output devices
- **PDFEngine**: Core PDF processing engine
- **PostScriptInterpreter**: PostScript language interpreter
- **ErrorHandler**: Error handling and reporting

### Key Enums

- **PDFQuality**: Screen, Default, Ebook, Printer, Prepress
- **PaperSize**: A4, Letter, Legal, A3, A5, Executive, Custom
- **DeviceType**: PDF_WRITE, MSWIN_PRINTER, POSTSCRIPT

### Error Handling

The library provides comprehensive error handling:

```cpp
ErrorInfo error = processor.GetLastError();
std::string error_msg = processor.GetLastErrorString();
```

## Integration

### C# Integration

```csharp
using System.Runtime.InteropServices;

public class PDFLib
{
    [DllImport("pdf_lib_c.dll")]
    public static extern IntPtr pdf_lib_create_processor();

    [DllImport("pdf_lib_c.dll")]
    public static extern int pdf_lib_initialize_with_options(IntPtr processor, ref Options options);

    // ... other P/Invoke declarations
}
```

### Python Integration

```python
import ctypes

pdf_lib = ctypes.CDLL('./libpdf_lib_c.so')

# Define structures and function signatures
# ... implementation details
```

## Performance

PDF_LIB is designed for performance with:

- Efficient memory management
- Minimal dependencies
- Optimized algorithms
- Cross-platform optimization

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests for new functionality
5. Submit a pull request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Inspired by Ghostscript's functionality
- Built with modern C++ best practices
- Designed for cross-platform compatibility

## Support

For issues, questions, or contributions:

- Create an issue on GitHub
- Check the documentation
- Review the examples

## Version History

- **1.0.0** - Initial release with core functionality
  - PDF processing engine
  - PostScript interpreter
  - Windows printing support
  - Command-line interface
  - C API wrapper
  - Comprehensive testing
