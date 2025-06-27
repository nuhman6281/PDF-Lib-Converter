/**
 * @file command_line_parser.cpp
 * @brief Command-line parser implementation
 * @version 1.0.0
 * @author PDF_LIB Team
 */

#include "command_line_parser.h"
#include "../core/error_handler.h"
#include "../utils/string_utils.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace PDFLib {

// Static member initialization
const std::map<std::string, std::string> CommandLineParser::known_flags_ = {
    {"-h", "Display help information"},
    {"--help", "Display help information"},
    {"-v", "Display version information"},
    {"--version", "Display version information"},
    {"-q", "Quiet mode (suppress output messages)"},
    {"--quiet", "Quiet mode (suppress output messages)"},
    {"-s", "Safer mode (disable file operations)"},
    {"--safer", "Safer mode (disable file operations)"},
    {"-d", "Define PostScript variable"},
    {"--define", "Define PostScript variable"},
    {"-r", "Set resolution in DPI"},
    {"--resolution", "Set resolution in DPI"},
    {"-g", "Set page size (width x height in pixels)"},
    {"--page-size", "Set page size (width x height in pixels)"},
    {"-dNOPAUSE", "Disable pausing between pages"},
    {"-dBATCH", "Process files and exit"},
    {"-dPrinted", "Mark output as printed"},
    {"-dEmptyStack", "Start with empty stack"},
    {"-dNOCACHE", "Disable caching"},
    {"-dNOPLATFONTS", "Disable platform fonts"},
    {"-dNOPROMPT", "Disable prompts"},
    {"-dQUIET", "Quiet mode"},
    {"-dSAFER", "Safer mode"},
    {"-dWRITESYSTEMDICT", "Write system dictionary"},
    {"-dWRITEUSERDICT", "Write user dictionary"},
    {"-dWRITEIMAGEDICT", "Write image dictionary"},
    {"-dWRITEFONTDICT", "Write font dictionary"},
    {"-dWRITEPATHDICT", "Write path dictionary"},
    {"-dWRITECOLORDICT", "Write color dictionary"},
    {"-dWRITEPAGEDICT", "Write page dictionary"},
    {"-dWRITECATALOG", "Write catalog"},
    {"-dWRITETRAILER", "Write trailer"},
    {"-dWRITEXREF", "Write cross-reference table"},
    {"-dWRITEOBJST", "Write object streams"},
    {"-dWRITESTRINGS", "Write strings"},
    {"-dWRITEARRAYS", "Write arrays"},
    {"-dWRITEDICTS", "Write dictionaries"},
    {"-dWRITENULLS", "Write null objects"},
    {"-dWRITEBOOLEANS", "Write boolean objects"},
    {"-dWRITEINTEGERS", "Write integer objects"},
    {"-dWRITEREALS", "Write real objects"},
    {"-dWRITENAMES", "Write name objects"},
    {"-dWRITEOPERATORS", "Write operator objects"},
    {"-dWRITEPROCEDURES", "Write procedure objects"},
    {"-dWRITEFILES", "Write file objects"},
    {"-dWRITESTREAMS", "Write stream objects"},
    {"-dWRITEGSTATES", "Write graphics state objects"},
    {"-dWRITECOLORSPACES", "Write color space objects"},
    {"-dWRITEPATTERNS", "Write pattern objects"},
    {"-dWRITESHADINGS", "Write shading objects"},
    {"-dWRITEXOBJECTS", "Write XObject objects"},
    {"-dWRITEFONTS", "Write font objects"},
    {"-dWRITEIMAGES", "Write image objects"},
    {"-dWRITEFORMS", "Write form objects"},
    {"-dWRITEANNOTATIONS", "Write annotation objects"},
    {"-dWRITEACTIONS", "Write action objects"},
    {"-dWRITENAMES", "Write name objects"},
    {"-dWRITEDESTINATIONS", "Write destination objects"},
    {"-dWRITEOUTLINES", "Write outline objects"},
    {"-dWRITETHREADS", "Write thread objects"},
    {"-dWRITEARTIFACTS", "Write artifact objects"},
    {"-dWRITEMETADATA", "Write metadata objects"},
    {"-dWRITEENCRYPTION", "Write encryption objects"},
    {"-dWRITEATTACHMENTS", "Write attachment objects"},
    {"-dWRITEALTERNATES", "Write alternate objects"},
    {"-dWRITEPRIVATE", "Write private objects"},
    {"-dWRITEUNKNOWN", "Write unknown objects"}
};

const std::map<std::string, std::string> CommandLineParser::known_devices_ = {
    {"pdfwrite", "PDF output device"},
    {"mswinpr2", "Windows printer device"},
    {"ps2write", "PostScript Level 2 output device"},
    {"pswrite", "PostScript Level 1 output device"},
    {"eps2write", "EPS Level 2 output device"},
    {"epswrite", "EPS Level 1 output device"},
    {"txtwrite", "Text output device"},
    {"png16m", "24-bit color PNG output device"},
    {"png256", "8-bit color PNG output device"},
    {"png16", "4-bit color PNG output device"},
    {"pnggray", "Grayscale PNG output device"},
    {"pngmono", "Monochrome PNG output device"},
    {"jpeg", "JPEG output device"},
    {"jpeggray", "Grayscale JPEG output device"},
    {"tiff12nc", "12-bit color TIFF output device"},
    {"tiff24nc", "24-bit color TIFF output device"},
    {"tiff48nc", "48-bit color TIFF output device"},
    {"tiff32nc", "32-bit color TIFF output device"},
    {"tiff64nc", "64-bit color TIFF output device"},
    {"tiffgray", "Grayscale TIFF output device"},
    {"tiffsep", "Separated TIFF output device"},
    {"tiffsep1", "Separated TIFF output device (1-bit)"},
    {"tiffcrle", "TIFF with CCITT RLE compression"},
    {"tifflzw", "TIFF with LZW compression"},
    {"tiffpack", "TIFF with PackBits compression"},
    {"tiffg3", "TIFF with CCITT G3 compression"},
    {"tiffg32d", "TIFF with CCITT G3 2D compression"},
    {"tiffg4", "TIFF with CCITT G4 compression"},
    {"bmp16m", "24-bit color BMP output device"},
    {"bmp256", "8-bit color BMP output device"},
    {"bmp16", "4-bit color BMP output device"},
    {"bmpgray", "Grayscale BMP output device"},
    {"bmpmono", "Monochrome BMP output device"},
    {"pcx16m", "24-bit color PCX output device"},
    {"pcx256", "8-bit color PCX output device"},
    {"pcx16", "4-bit color PCX output device"},
    {"pcxgray", "Grayscale PCX output device"},
    {"pcxmono", "Monochrome PCX output device"},
    {"pbm", "Portable Bitmap output device"},
    {"pgm", "Portable Graymap output device"},
    {"ppm", "Portable Pixmap output device"},
    {"pnm", "Portable Anymap output device"},
    {"pkm", "Portable Kmap output device"},
    {"pam", "Portable Arbitrary Map output device"},
    {"pamcmyk32", "32-bit CMYK PAM output device"},
    {"pamcmyk4", "4-bit CMYK PAM output device"},
    {"pamcmyk", "CMYK PAM output device"},
    {"pamrgb", "RGB PAM output device"},
    {"pamgray", "Grayscale PAM output device"},
    {"pammono", "Monochrome PAM output device"},
    {"pamcmyk32", "32-bit CMYK PAM output device"},
    {"pamcmyk4", "4-bit CMYK PAM output device"},
    {"pamcmyk", "CMYK PAM output device"},
    {"pamrgb", "RGB PAM output device"},
    {"pamgray", "Grayscale PAM output device"},
    {"pammono", "Monochrome PAM output device"}
};

CommandLineParser::CommandLineParser() {
}

CommandLineParser::~CommandLineParser() {
}

bool CommandLineParser::Parse(const std::vector<std::string>& arguments, ProcessingOptions& options) {
    // Reset options to defaults
    options = ProcessingOptions{};
    
    for (size_t i = 0; i < arguments.size(); ++i) {
        const std::string& arg = arguments[i];
        const std::string& next_arg = (i + 1 < arguments.size()) ? arguments[i + 1] : "";
        bool has_next = (i + 1 < arguments.size());
        
        if (!ParseArgument(arg, options, next_arg, has_next)) {
            return false;
        }
        
        // Skip next argument if current argument consumed it
        if (has_next && !IsFlag(next_arg) && !IsDevice(next_arg)) {
            i++;
        }
    }
    
    return ValidateOptions(options);
}

bool CommandLineParser::Parse(int argc, char* argv[], ProcessingOptions& options) {
    std::vector<std::string> arguments;
    for (int i = 0; i < argc; ++i) {
        arguments.push_back(argv[i]);
    }
    
    return Parse(arguments, options);
}

std::string CommandLineParser::GetHelpText() {
    std::ostringstream help;
    help << "PDF_LIB - A lightweight Ghostscript alternative\n";
    help << "Usage: pdf_gs [options] [input_files]\n\n";
    help << "Options:\n";
    help << "  -h, --help                    Display this help information\n";
    help << "  -v, --version                 Display version information\n";
    help << "  -q, --quiet                   Suppress output messages\n";
    help << "  -s, --safer                   Enable safer mode\n";
    help << "  -dNOPAUSE                     Disable pausing between pages\n";
    help << "  -dBATCH                       Process files and exit\n";
    help << "  -dPrinted                     Mark output as printed\n";
    help << "  -dEmptyStack                  Start with empty stack\n";
    help << "  -sDEVICE=<device>             Set output device\n";
    help << "  -sOutputFile=<file>           Set output file\n";
    help << "  -dCompatibilityLevel=<level>  Set PDF compatibility level\n";
    help << "  -dNumCopies=<copies>          Set number of copies\n";
    help << "  -sPaperSize=<size>            Set paper size\n";
    help << "  -g<width>x<height>            Set page dimensions in pixels\n";
    help << "  -r<dpi>                       Set resolution in DPI\n";
    help << "  -dMargins=<left,top,right,bottom>  Set page margins\n";
    help << "  -dPageOffset=<x,y>            Set page offset\n";
    help << "  -dQuality=<quality>           Set output quality\n";
    help << "  -sPrinterName=<name>          Set printer name (Windows)\n\n";
    help << "Devices:\n";
    for (const auto& device : known_devices_) {
        help << "  " << device.first << " - " << device.second << "\n";
    }
    help << "\nExamples:\n";
    help << "  pdf_gs -sDEVICE=pdfwrite -sOutputFile=output.pdf input.ps\n";
    help << "  pdf_gs -sDEVICE=mswinpr2 -sPrinterName=\"HP LaserJet\" input.pdf\n";
    help << "  pdf_gs -sDEVICE=png16m -r300 -g2480x3508 input.pdf\n";
    
    return help.str();
}

std::string CommandLineParser::GetVersionText() {
    return "PDF_LIB version 1.0.0 - A lightweight Ghostscript alternative";
}

bool CommandLineParser::IsFlag(const std::string& arg) {
    return arg.length() > 1 && (arg[0] == '-' || arg[0] == '/');
}

bool CommandLineParser::IsDevice(const std::string& arg) {
    return known_devices_.find(arg) != known_devices_.end();
}

bool CommandLineParser::ParseArgument(const std::string& arg, ProcessingOptions& options, 
                                      const std::string& next_arg, bool has_next) {
    if (arg.empty()) {
        return true;
    }
    
    // Handle flags
    if (IsFlag(arg)) {
        if (arg == "-h" || arg == "--help") {
            std::cout << GetHelpText() << std::endl;
            return false; // Exit after showing help
        }
        
        if (arg == "-v" || arg == "--version") {
            std::cout << GetVersionText() << std::endl;
            return false; // Exit after showing version
        }
        
        if (arg == "-q" || arg == "--quiet") {
            options.quiet = true;
            return true;
        }
        
        if (arg == "-s" || arg == "--safer") {
            options.safer_mode = true;
            return true;
        }
        
        if (arg == "-dNOPAUSE") {
            options.no_pause = true;
            return true;
        }
        
        if (arg == "-dBATCH") {
            options.batch_mode = true;
            return true;
        }
        
        if (arg == "-dPrinted") {
            options.printed = true;
            return true;
        }
        
        if (arg == "-dEmptyStack") {
            options.empty_stack = true;
            return true;
        }
        
        // Handle device specification
        if (arg.substr(0, 8) == "-sDEVICE=") {
            std::string device = arg.substr(8);
            return ParseDevice(device, options);
        }
        
        // Handle output file specification
        if (arg.substr(0, 12) == "-sOutputFile=") {
            options.output_file = arg.substr(12);
            return true;
        }
        
        // Handle compatibility level
        if (arg.substr(0, 20) == "-dCompatibilityLevel=") {
            try {
                options.compatibility_level = std::stod(arg.substr(20));
                return true;
            } catch (const std::exception& e) {
                ErrorHandler::GetInstance().SetError(-1, "Invalid compatibility level: " + arg.substr(20));
                return false;
            }
        }
        
        // Handle number of copies
        if (arg.substr(0, 12) == "-dNumCopies=") {
            try {
                options.num_copies = std::stoi(arg.substr(12));
                return true;
            } catch (const std::exception& e) {
                ErrorHandler::GetInstance().SetError(-1, "Invalid number of copies: " + arg.substr(12));
                return false;
            }
        }
        
        // Handle paper size
        if (arg.substr(0, 12) == "-sPaperSize=") {
            std::string size = arg.substr(12);
            return ParsePaperSize(size, options);
        }
        
        // Handle page dimensions
        if (arg.size() > 1 && arg[0] == '-' && arg[1] == 'g') {
            std::string dims = arg.substr(1);
            return ParseCustomDimensions(dims, options);
        }
        
        // Handle resolution
        if (arg.size() > 1 && arg[0] == '-' && arg[1] == 'r') {
            std::string res = arg.substr(1);
            try {
                double dpi = std::stod(res);
                // Convert DPI to quality setting
                if (dpi <= 72) options.quality = PDFQuality::SCREEN;
                else if (dpi <= 150) options.quality = PDFQuality::EBOOK;
                else if (dpi <= 300) options.quality = PDFQuality::PRINTER;
                else options.quality = PDFQuality::PREPRESS;
                return true;
            } catch (const std::exception& e) {
                ErrorHandler::GetInstance().SetError(-1, "Invalid resolution: " + res);
                return false;
            }
        }
        
        // Handle margins
        if (arg.substr(0, 10) == "-dMargins=") {
            std::string margins = arg.substr(10);
            return ParseMargins(margins, options);
        }
        
        // Handle page offset
        if (arg.substr(0, 13) == "-dPageOffset=") {
            std::string offset = arg.substr(13);
            return ParsePageOffset(offset, options);
        }
        
        // Handle quality
        if (arg.substr(0, 10) == "-dQuality=") {
            std::string quality = arg.substr(10);
            return ParseQuality(quality, options);
        }
        
        // Handle printer name (Windows)
        if (arg.substr(0, 15) == "-sPrinterName=") {
            options.printer_name = arg.substr(15);
            return true;
        }
        
        // Unknown flag
        ErrorHandler::GetInstance().SetError(-1, "Unknown flag: " + arg);
        return false;
    }
    
    // Handle device names
    if (IsDevice(arg)) {
        return ParseDevice(arg, options);
    }
    
    // Handle input files
    options.input_files.push_back(arg);
    return true;
}

bool CommandLineParser::ParseQuality(const std::string& value, ProcessingOptions& options) {
    std::string lower_value = Utils::ToLower(value);
    
    if (lower_value == "screen" || lower_value == "72") {
        options.quality = PDFQuality::SCREEN;
    } else if (lower_value == "default" || lower_value == "150") {
        options.quality = PDFQuality::DEFAULT;
    } else if (lower_value == "ebook" || lower_value == "150") {
        options.quality = PDFQuality::EBOOK;
    } else if (lower_value == "printer" || lower_value == "300") {
        options.quality = PDFQuality::PRINTER;
    } else if (lower_value == "prepress" || lower_value == "300") {
        options.quality = PDFQuality::PREPRESS;
    } else {
        ErrorHandler::GetInstance().SetError(-1, "Invalid quality setting: " + value);
        return false;
    }
    
    return true;
}

bool CommandLineParser::ParsePaperSize(const std::string& value, ProcessingOptions& options) {
    std::string lower_value = Utils::ToLower(value);
    
    if (lower_value == "a4") {
        options.paper_size = PaperSize::A4;
    } else if (lower_value == "letter") {
        options.paper_size = PaperSize::LETTER;
    } else if (lower_value == "legal") {
        options.paper_size = PaperSize::LEGAL;
    } else if (lower_value == "a3") {
        options.paper_size = PaperSize::A3;
    } else if (lower_value == "a5") {
        options.paper_size = PaperSize::A5;
    } else if (lower_value == "executive") {
        options.paper_size = PaperSize::EXECUTIVE;
    } else if (lower_value == "custom") {
        options.paper_size = PaperSize::CUSTOM;
    } else {
        ErrorHandler::GetInstance().SetError(-1, "Invalid paper size: " + value);
        return false;
    }
    
    return true;
}

bool CommandLineParser::ParseDevice(const std::string& value, ProcessingOptions& options) {
    if (known_devices_.find(value) == known_devices_.end()) {
        ErrorHandler::GetInstance().SetError(-1, "Unknown device: " + value);
        return false;
    }
    
    options.device_name = value;
    return true;
}

bool CommandLineParser::ParseMargins(const std::string& value, ProcessingOptions& options) {
    std::vector<std::string> parts = Utils::Split(value, ',');
    if (parts.size() != 4) {
        ErrorHandler::GetInstance().SetError(-1, "Invalid margins format: " + value + " (expected: left,top,right,bottom)");
        return false;
    }
    
    try {
        options.left_margin = std::stod(parts[0]);
        options.top_margin = std::stod(parts[1]);
        options.right_margin = std::stod(parts[2]);
        options.bottom_margin = std::stod(parts[3]);
    } catch (const std::exception& e) {
        ErrorHandler::GetInstance().SetError(-1, "Invalid margin values: " + value);
        return false;
    }
    
    return true;
}

bool CommandLineParser::ParsePageOffset(const std::string& value, ProcessingOptions& options) {
    std::vector<std::string> parts = Utils::Split(value, ',');
    if (parts.size() != 2) {
        ErrorHandler::GetInstance().SetError(-1, "Invalid page offset format: " + value + " (expected: x,y)");
        return false;
    }
    
    try {
        options.page_offset_x = std::stod(parts[0]);
        options.page_offset_y = std::stod(parts[1]);
    } catch (const std::exception& e) {
        ErrorHandler::GetInstance().SetError(-1, "Invalid page offset values: " + value);
        return false;
    }
    
    return true;
}

bool CommandLineParser::ParseCustomDimensions(const std::string& value, ProcessingOptions& options) {
    size_t x_pos = value.find('x');
    if (x_pos == std::string::npos) {
        ErrorHandler::GetInstance().SetError(-1, "Invalid dimensions format: " + value + " (expected: widthxheight)");
        return false;
    }
    
    try {
        std::string width_str = value.substr(0, x_pos);
        std::string height_str = value.substr(x_pos + 1);
        
        options.custom_width_points = std::stod(width_str);
        options.custom_height_points = std::stod(height_str);
        options.paper_size = PaperSize::CUSTOM;
    } catch (const std::exception& e) {
        ErrorHandler::GetInstance().SetError(-1, "Invalid dimension values: " + value);
        return false;
    }
    
    return true;
}

bool CommandLineParser::ValidateOptions(const ProcessingOptions& options) {
    if (options.device_name.empty()) {
        ErrorHandler::GetInstance().SetError(-1, "No output device specified");
        return false;
    }
    
    if (options.input_files.empty()) {
        ErrorHandler::GetInstance().SetError(-1, "No input files specified");
        return false;
    }
    
    if (options.compatibility_level < 1.0 || options.compatibility_level > 2.0) {
        ErrorHandler::GetInstance().SetError(-1, "Invalid compatibility level: " + std::to_string(options.compatibility_level));
        return false;
    }
    
    if (options.num_copies < 1) {
        ErrorHandler::GetInstance().SetError(-1, "Invalid number of copies: " + std::to_string(options.num_copies));
        return false;
    }
    
    return true;
}

} // namespace PDFLib 