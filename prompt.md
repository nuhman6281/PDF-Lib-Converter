# Comprehensive C++ Library Development Prompt: Ghostscript Alternative

## Project Overview

I need to develop a native C++ library that provides core Ghostscript functionality focused on PDF processing and printing operations. This library should serve as a lightweight alternative to Ghostscript, specifically supporting PDF-to-printer conversion and PDF manipulation tasks.

## Core Requirements

### 1. **Primary Device Support**

- **pdfwrite device**: PDF creation, manipulation, and optimization
- **mswinpr2 device**: Windows printer driver integration for direct PDF printing
- Support for PostScript (.ps) and Encapsulated PostScript (.eps) file processing
- Temporary file handling for Windows printer spooling operations

### 2. **Essential Command Line Parameters**

The library must support these critical Ghostscript-equivalent parameters:

#### Batch and Control Parameters

- `-dBATCH`: Process files and exit
- `-dNOPAUSE`: Disable pausing between pages
- `-dQUIET`: Suppress output messages
- `-dSAFER`: Enable safe mode operations
- `-dNOSAFER`: Disable safe mode (when needed)
- `-dPrinted`: Mark output as printed
- `-empty`: Start with empty stack

#### PDF-Specific Parameters

- `-dAutoRotatePages=/None`: Control page rotation
- `-dPDFSETTINGS=/screen|/default|/ebook|/printer|/prepress`: Quality settings
- `-dCompatibilityLevel=X.X`: PDF version compatibility
- `-dNumCopies=N`: Number of copies for printing

#### Device and Output Parameters

- `-sDEVICE=devicename`: Specify output device (pdfwrite, mswinpr2)
- `-sOutputFile=path`: Output file path
- `-sPAPERSIZE=custom|a4|letter`: Paper size specification
- `-dDEVICEWIDTHPOINTS=N`: Custom width in points
- `-dDEVICEHEIGHTPOINTS=N`: Custom height in points

#### Margin and Positioning Parameters

- `-dPDFX11LeftMargin=N`: Left margin control
- Custom PageOffset support: `<< /PageOffset [x y] >> setpagedevice`
- Custom Margins support: `<< /Margins [x y] >> setpagedevice`

#### PostScript Command Integration

- `-c "postscript_commands"`: Execute PostScript commands
- `-f`: Process files after commands

### 3. **Windows Printer Integration**

- Direct integration with Windows printing subsystem
- Support for `%printer%PrinterName` syntax for direct printer output
- Handle Windows GDI/GDI+ for printer communication
- Support custom paper sizes and printer-specific settings
- Integration with Windows Driver Kit (WDK) if necessary for low-level printer access

### 4. **File Format Support**

- **Input formats**: PDF, PostScript (.ps), Encapsulated PostScript (.eps)
- **Output formats**: PDF, direct printer output, temporary printer files
- Unicode (UTF-8) file path and content handling
- Robust file I/O with proper error handling

### 5. **API Design Requirements**

Create a C++ library with these components:

#### Core API Structure

```cpp
// Main processor class
class PDFProcessor {
public:
    // Initialize with command-line style arguments
    bool Initialize(const std::vector<std::string>& arguments);

    // Execute processing operation
    bool Process();

    // Cleanup resources
    void Cleanup();

    // Error handling
    std::string GetLastError();
};

// Device-specific handlers
class PDFWriteDevice;
class MSWinPrinterDevice;
class PostScriptProcessor;
```

#### Memory Management

- Efficient memory handling for large PDF files
- Streaming processing capabilities for large documents
- Proper cleanup of native resources
- Exception-safe operations

## Technical Implementation Requirements

### 1. **Core Dependencies and Libraries**

- **PDF Processing**: Integrate or develop PDF parsing/writing capabilities (consider PDFium, PoDoFo, or custom implementation)
- **PostScript Interpretation**: Basic PostScript command processor for margin/offset commands
- **Windows API Integration**: GDI/GDI+ for printer operations, Spooler API for printer management
- **File I/O**: Robust file handling with Unicode support
- **String Processing**: Command-line argument parsing and PostScript command interpretation

### 2. **Architecture Design**

- **Command Parser**: Parse Ghostscript-style command line arguments
- **Device Manager**: Factory pattern for different output devices (PDF, printer)
- **PostScript Interpreter**: Limited interpreter for essential PostScript commands
- **Resource Manager**: Handle temporary files, memory, and system resources
- **Error Handler**: Comprehensive error reporting and logging

### 3. **Windows-Specific Implementation**

- Use Windows Printing API for `mswinpr2` device implementation
- Integrate with Windows spooler service
- Handle printer enumeration and capability detection
- Support for Windows-specific paper sizes and printer settings
- Consider WDK integration for advanced printer control if needed

### 4. **Cross-Platform Considerations**

- Design core architecture to be platform-independent
- Isolate Windows-specific printer code in separate modules
- Use conditional compilation for platform-specific features

## Implementation Guidelines

### 1. **Development Approach**

- Start with core PDF reading/writing functionality
- Implement command-line argument parser
- Add `pdfwrite` device support first
- Implement `mswinpr2` device with Windows printer integration
- Add PostScript command interpretation
- Implement margin and positioning features

### 2. **Code Organization**

```
/src
  /core          - Core PDF processing engine
  /devices       - Device-specific implementations
  /parsers       - Command-line and PostScript parsers
  /platform      - Platform-specific code (Windows/Linux)
  /utils         - Utility functions and helpers
/include         - Public header files
/tests           - Unit tests and integration tests
/examples        - Usage examples and documentation
```

### 3. **Build System**

- Use CMake for cross-platform build support
- Support both static and dynamic library builds
- Include proper dependency management
- Support Visual Studio on Windows and GCC/Clang on Linux

### 4. **Testing Requirements**

- Unit tests for each major component
- Integration tests with real PDF files
- Windows printer testing with various printer types
- Performance benchmarks against original Ghostscript
- Memory leak detection and performance profiling

## Deliverables Expected

1. **Core C++ Library** with all specified functionality
2. **C-style API wrapper** for easy integration with other languages
3. **Command-line executable** that mimics Ghostscript interface
4. **Comprehensive documentation** with API reference and usage examples
5. **Test suite** with sample PDF files and test cases
6. **Build scripts** for Windows and Linux platforms

## Success Criteria

- Successfully process the provided C# code's PDF printing scenarios
- Handle custom paper sizes and margins as shown in the examples
- Provide equivalent functionality to the Ghostscript commands used
- Maintain good performance (within 2x of Ghostscript processing time)
- Stable memory usage without leaks
- Proper error handling and reporting

## Additional Notes

- Focus on the specific use cases shown in the provided code rather than full Ghostscript compatibility
- Prioritize reliability and performance over feature completeness
- Ensure the library can be easily integrated into existing C# applications via P/Invoke
- Consider future extensibility for additional devices or formats

Please generate a complete, production-ready C++ library implementation that fulfills these requirements, with proper error handling, documentation, and example usage code.
