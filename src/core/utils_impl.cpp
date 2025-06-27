/**
 * @file utils_impl.cpp
 * @brief Utility functions implementation for PDF_LIB
 */

#include "../include/pdf_lib.h"
#include "utils/file_utils.h"
#include "utils/string_utils.h"
#include <algorithm>
#include <sstream>

namespace PDFLib {

namespace Utils {

ProcessingOptions ParseArguments(const std::vector<std::string>& arguments) {
    // This is a simplified implementation
    // In a real implementation, you would use the CommandParser class
    ProcessingOptions options;
    
    for (const auto& arg : arguments) {
        if (arg == "-dBATCH") {
            options.batch_mode = true;
        } else if (arg == "-dNOPAUSE") {
            options.no_pause = true;
        } else if (arg == "-dQUIET") {
            options.quiet = true;
        } else if (arg == "-dSAFER") {
            options.safer_mode = true;
        } else if (arg == "-dNOSAFER") {
            options.safer_mode = false;
        } else if (arg == "-dPrinted") {
            options.printed = true;
        } else if (arg == "-empty") {
            options.empty_stack = true;
        } else if (arg.find("-dPDFSETTINGS=") == 0) {
            std::string value = arg.substr(13);
            options.quality = StringToQuality(value);
        } else if (arg.find("-dCompatibilityLevel=") == 0) {
            std::string value = arg.substr(20);
            try {
                options.compatibility_level = std::stod(value);
            } catch (...) {
                // Use default value
            }
        } else if (arg.find("-dNumCopies=") == 0) {
            std::string value = arg.substr(11);
            try {
                options.num_copies = std::stoi(value);
            } catch (...) {
                // Use default value
            }
        } else if (arg.find("-sDEVICE=") == 0) {
            options.device_name = arg.substr(9);
        } else if (arg.find("-sOutputFile=") == 0) {
            options.output_file = arg.substr(12);
        } else if (arg.find("-sPAPERSIZE=") == 0) {
            std::string value = arg.substr(11);
            options.paper_size = StringToPaperSize(value);
        } else if (arg.find("-dDEVICEWIDTHPOINTS=") == 0) {
            std::string value = arg.substr(19);
            try {
                options.custom_width_points = std::stod(value);
            } catch (...) {
                // Use default value
            }
        } else if (arg.find("-dDEVICEHEIGHTPOINTS=") == 0) {
            std::string value = arg.substr(20);
            try {
                options.custom_height_points = std::stod(value);
            } catch (...) {
                // Use default value
            }
        } else if (arg.find("-dPDFX11LeftMargin=") == 0) {
            std::string value = arg.substr(18);
            try {
                options.left_margin = std::stod(value);
            } catch (...) {
                // Use default value
            }
        } else if (!arg.empty() && arg[0] != '-') {
            // Assume it's an input file
            options.input_files.push_back(arg);
        }
    }
    
    return options;
}

std::string QualityToString(PDFQuality quality) {
    switch (quality) {
        case PDFQuality::SCREEN: return "screen";
        case PDFQuality::EBOOK: return "ebook";
        case PDFQuality::PRINTER: return "printer";
        case PDFQuality::PREPRESS: return "prepress";
        default: return "default";
    }
}

PDFQuality StringToQuality(const std::string& str) {
    std::string lower = ToLower(str);
    if (lower == "screen") return PDFQuality::SCREEN;
    if (lower == "ebook") return PDFQuality::EBOOK;
    if (lower == "printer") return PDFQuality::PRINTER;
    if (lower == "prepress") return PDFQuality::PREPRESS;
    return PDFQuality::DEFAULT;
}

std::string PaperSizeToString(PaperSize size) {
    switch (size) {
        case PaperSize::CUSTOM: return "custom";
        case PaperSize::LETTER: return "letter";
        case PaperSize::LEGAL: return "legal";
        case PaperSize::A3: return "a3";
        case PaperSize::A5: return "a5";
        case PaperSize::EXECUTIVE: return "executive";
        default: return "a4";
    }
}

PaperSize StringToPaperSize(const std::string& str) {
    std::string lower = ToLower(str);
    if (lower == "custom") return PaperSize::CUSTOM;
    if (lower == "letter") return PaperSize::LETTER;
    if (lower == "legal") return PaperSize::LEGAL;
    if (lower == "a3") return PaperSize::A3;
    if (lower == "a5") return PaperSize::A5;
    if (lower == "executive") return PaperSize::EXECUTIVE;
    return PaperSize::A4;
}

void GetPaperDimensions(PaperSize size, double& width, double& height) {
    // Dimensions in points (1 point = 1/72 inch)
    switch (size) {
        case PaperSize::A4:
            width = 595.276;   // 210mm
            height = 841.890;  // 297mm
            break;
        case PaperSize::LETTER:
            width = 612.000;   // 8.5 inches
            height = 792.000;  // 11 inches
            break;
        case PaperSize::LEGAL:
            width = 612.000;   // 8.5 inches
            height = 1008.000; // 14 inches
            break;
        case PaperSize::A3:
            width = 841.890;   // 297mm
            height = 1190.551; // 420mm
            break;
        case PaperSize::A5:
            width = 419.528;   // 148mm
            height = 595.276;  // 210mm
            break;
        case PaperSize::EXECUTIVE:
            width = 522.000;   // 7.25 inches
            height = 756.000;  // 10.5 inches
            break;
        case PaperSize::CUSTOM:
        default:
            width = 0.0;
            height = 0.0;
            break;
    }
}

// These functions are already implemented in file_utils.cpp
// No need to duplicate them here

} // namespace Utils

} // namespace PDFLib 