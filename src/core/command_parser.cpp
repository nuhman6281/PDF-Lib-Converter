/**
 * @file command_parser.cpp
 * @brief Command-line argument parser implementation
 */

#include "command_parser.h"
#include <algorithm>
#include <sstream>
#include <cctype>
#include <cstring>

namespace PDFLib {

class CommandParser::Impl {
public:
    Impl() {
        InitializeParameterDefinitions();
    }
    
    void InitializeParameterDefinitions() {
        // Batch and control parameters
        parameters_.push_back({"BATCH", "dBATCH", ParameterType::BOOLEAN, 
                             "Process files and exit", "false", false, {}});
        parameters_.push_back({"NOPAUSE", "dNOPAUSE", ParameterType::BOOLEAN, 
                             "Disable pausing between pages", "false", false, {}});
        parameters_.push_back({"QUIET", "dQUIET", ParameterType::BOOLEAN, 
                             "Suppress output messages", "false", false, {}});
        parameters_.push_back({"SAFER", "dSAFER", ParameterType::BOOLEAN, 
                             "Enable safe mode operations", "true", false, {}});
        parameters_.push_back({"NOSAFER", "dNOSAFER", ParameterType::BOOLEAN, 
                             "Disable safe mode", "false", false, {}});
        parameters_.push_back({"PRINTED", "dPrinted", ParameterType::BOOLEAN, 
                             "Mark output as printed", "false", false, {}});
        parameters_.push_back({"EMPTY", "empty", ParameterType::BOOLEAN, 
                             "Start with empty stack", "false", false, {}});
        
        // PDF-specific parameters
        parameters_.push_back({"AUTOROTATEPAGES", "dAutoRotatePages", ParameterType::STRING, 
                             "Control page rotation", "None", false, {"None", "All", "PageByPage"}});
        parameters_.push_back({"PDFSETTINGS", "dPDFSETTINGS", ParameterType::QUALITY, 
                             "PDF quality settings", "default", false, {"screen", "default", "ebook", "printer", "prepress"}});
        parameters_.push_back({"COMPATIBILITYLEVEL", "dCompatibilityLevel", ParameterType::DOUBLE, 
                             "PDF version compatibility", "1.7", false, {}});
        parameters_.push_back({"NUMCOPIES", "dNumCopies", ParameterType::INTEGER, 
                             "Number of copies for printing", "1", false, {}});
        
        // Device and output parameters
        parameters_.push_back({"DEVICE", "sDEVICE", ParameterType::DEVICE_TYPE, 
                             "Specify output device", "pdfwrite", false, {"pdfwrite", "mswinpr2"}});
        parameters_.push_back({"OUTPUTFILE", "sOutputFile", ParameterType::STRING, 
                             "Output file path", "", false, {}});
        parameters_.push_back({"PAPERSIZE", "sPAPERSIZE", ParameterType::PAPER_SIZE, 
                             "Paper size specification", "a4", false, {"custom", "a4", "letter", "legal", "a3", "a5", "executive"}});
        parameters_.push_back({"DEVICEWIDTHPOINTS", "dDEVICEWIDTHPOINTS", ParameterType::DOUBLE, 
                             "Custom width in points", "0.0", false, {}});
        parameters_.push_back({"DEVICEHEIGHTPOINTS", "dDEVICEHEIGHTPOINTS", ParameterType::DOUBLE, 
                             "Custom height in points", "0.0", false, {}});
        
        // Margin and positioning parameters
        parameters_.push_back({"PDFX11LEFTMARGIN", "dPDFX11LeftMargin", ParameterType::DOUBLE, 
                             "Left margin control", "0.0", false, {}});
        parameters_.push_back({"PAGEOFFSETX", "PageOffsetX", ParameterType::DOUBLE, 
                             "Page offset X in points", "0.0", false, {}});
        parameters_.push_back({"PAGEOFFSETY", "PageOffsetY", ParameterType::DOUBLE, 
                             "Page offset Y in points", "0.0", false, {}});
        parameters_.push_back({"MARGINLEFT", "MarginLeft", ParameterType::DOUBLE, 
                             "Left margin in points", "0.0", false, {}});
        parameters_.push_back({"MARGINRIGHT", "MarginRight", ParameterType::DOUBLE, 
                             "Right margin in points", "0.0", false, {}});
        parameters_.push_back({"MARGINTOP", "MarginTop", ParameterType::DOUBLE, 
                             "Top margin in points", "0.0", false, {}});
        parameters_.push_back({"MARGINBOTTOM", "MarginBottom", ParameterType::DOUBLE, 
                             "Bottom margin in points", "0.0", false, {}});
    }
    
    bool ParseArguments(const std::vector<std::string>& arguments, 
                       ProcessingOptions& options) {
        std::map<std::string, ParameterValue> params;
        unrecognized_args_.clear();
        
        // Parse each argument
        for (size_t i = 0; i < arguments.size(); ++i) {
            const std::string& arg = arguments[i];
            
            // Skip empty arguments
            if (arg.empty()) {
                continue;
            }
            
            // Check for PostScript commands (-c)
            if (arg == "-c" && i + 1 < arguments.size()) {
                options.postscript_commands.push_back(arguments[i + 1]);
                i++; // Skip next argument as it's the command
                continue;
            }
            
            // Check for file processing flag (-f)
            if (arg == "-f") {
                // This indicates that files follow
                continue;
            }
            
            // Check for input files (not starting with -)
            if (!StartsWith(arg, "-")) {
                options.input_files.push_back(arg);
                continue;
            }
            
            // Parse parameter
            if (!ParseArgument(arg, static_cast<int>(i), params)) {
                unrecognized_args_.push_back(arg);
            }
        }
        
        // Convert parameters to options
        return ConvertToOptions(params, options);
    }
    
    bool ParseArgument(const std::string& arg, int position, 
                      std::map<std::string, ParameterValue>& params) {
        std::string param_name = arg.substr(1); // Remove leading -
        std::string value;
        
        // Check if parameter has a value (contains =)
        size_t equal_pos = param_name.find('=');
        if (equal_pos != std::string::npos) {
            value = param_name.substr(equal_pos + 1);
            param_name = param_name.substr(0, equal_pos);
        }
        
        // Find parameter definition
        const ParameterDefinition* param_def = FindParameterDefinition(param_name);
        if (!param_def) {
            return false;
        }
        
        // Create parameter value
        ParameterValue param_value;
        param_value.name = param_def->name;
        param_value.is_set = true;
        param_value.position = position;
        
        // Parse value
        if (!ParseParameterValue(*param_def, value, param_value)) {
            return false;
        }
        
        params[param_def->name] = param_value;
        return true;
    }
    
    bool ParseParameterValue(const ParameterDefinition& param_def,
                           const std::string& value_str,
                           ParameterValue& value) {
        switch (param_def.type) {
            case ParameterType::BOOLEAN:
                value.value = value_str.empty() ? "true" : value_str;
                break;
                
            case ParameterType::STRING:
                value.value = value_str;
                break;
                
            case ParameterType::INTEGER:
                if (!value_str.empty()) {
                    try {
                        std::stoi(value_str);
                        value.value = value_str;
                    } catch (...) {
                        ErrorHandler::GetInstance().SetError(-1, "Invalid integer value: " + value_str);
                        return false;
                    }
                }
                break;
                
            case ParameterType::DOUBLE:
                if (!value_str.empty()) {
                    try {
                        std::stod(value_str);
                        value.value = value_str;
                    } catch (...) {
                        ErrorHandler::GetInstance().SetError(-1, "Invalid double value: " + value_str);
                        return false;
                    }
                }
                break;
                
            case ParameterType::QUALITY:
            case ParameterType::PAPER_SIZE:
            case ParameterType::DEVICE_TYPE:
                if (!value_str.empty()) {
                    std::string lower_value = ToLower(value_str);
                    if (std::find(param_def.allowed_values.begin(), 
                                 param_def.allowed_values.end(), 
                                 lower_value) != param_def.allowed_values.end()) {
                        value.value = lower_value;
                    } else {
                        ErrorHandler::GetInstance().SetError(-1, "Invalid value for " + param_def.name + ": " + value_str);
                        return false;
                    }
                }
                break;
        }
        
        return true;
    }
    
    bool ConvertToOptions(const std::map<std::string, ParameterValue>& params,
                         ProcessingOptions& options) {
        // Set default values
        options = ProcessingOptions{};
        
        // Convert parameters to options
        for (const auto& pair : params) {
            const std::string& name = pair.first;
            const ParameterValue& value = pair.second;
            
            if (name == "BATCH") {
                options.batch_mode = (value.value == "true");
            } else if (name == "NOPAUSE") {
                options.no_pause = (value.value == "true");
            } else if (name == "QUIET") {
                options.quiet = (value.value == "true");
            } else if (name == "SAFER") {
                options.safer_mode = (value.value == "true");
            } else if (name == "NOSAFER") {
                options.safer_mode = (value.value == "false");
            } else if (name == "PRINTED") {
                options.printed = (value.value == "true");
            } else if (name == "EMPTY") {
                options.empty_stack = (value.value == "true");
            } else if (name == "PDFSETTINGS") {
                options.quality = StringToQuality(value.value);
            } else if (name == "COMPATIBILITYLEVEL") {
                if (!value.value.empty()) {
                    options.compatibility_level = std::stod(value.value);
                }
            } else if (name == "NUMCOPIES") {
                if (!value.value.empty()) {
                    options.num_copies = std::stoi(value.value);
                }
            } else if (name == "DEVICE") {
                options.device_name = value.value;
            } else if (name == "OUTPUTFILE") {
                options.output_file = value.value;
            } else if (name == "PAPERSIZE") {
                options.paper_size = StringToPaperSize(value.value);
            } else if (name == "DEVICEWIDTHPOINTS") {
                if (!value.value.empty()) {
                    options.custom_width_points = std::stod(value.value);
                }
            } else if (name == "DEVICEHEIGHTPOINTS") {
                if (!value.value.empty()) {
                    options.custom_height_points = std::stod(value.value);
                }
            } else if (name == "PDFX11LEFTMARGIN") {
                if (!value.value.empty()) {
                    options.left_margin = std::stod(value.value);
                }
            } else if (name == "PAGEOFFSETX") {
                if (!value.value.empty()) {
                    options.page_offset_x = std::stod(value.value);
                }
            } else if (name == "PAGEOFFSETY") {
                if (!value.value.empty()) {
                    options.page_offset_y = std::stod(value.value);
                }
            } else if (name == "MARGINLEFT") {
                if (!value.value.empty()) {
                    options.left_margin = std::stod(value.value);
                }
            } else if (name == "MARGINRIGHT") {
                if (!value.value.empty()) {
                    options.right_margin = std::stod(value.value);
                }
            } else if (name == "MARGINTOP") {
                if (!value.value.empty()) {
                    options.top_margin = std::stod(value.value);
                }
            } else if (name == "MARGINBOTTOM") {
                if (!value.value.empty()) {
                    options.bottom_margin = std::stod(value.value);
                }
            }
        }
        
        return true;
    }
    
    const ParameterDefinition* FindParameterDefinition(const std::string& name) const {
        for (const auto& param : parameters_) {
            if (ToLower(param.name) == ToLower(name) || 
                ToLower(param.short_name) == ToLower(name)) {
                return &param;
            }
        }
        return nullptr;
    }
    
    bool StartsWith(const std::string& str, const std::string& prefix) const {
        if (str.length() < prefix.length()) {
            return false;
        }
        return str.compare(0, prefix.length(), prefix) == 0;
    }
    
    std::vector<std::string> SplitString(const std::string& str, char delimiter) const {
        std::vector<std::string> result;
        std::stringstream ss(str);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            result.push_back(TrimString(item));
        }
        return result;
    }
    
    std::string TrimString(const std::string& str) const {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            return "";
        }
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    std::string ToLower(const std::string& str) const {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    PDFQuality StringToQuality(const std::string& str) const {
        std::string lower = ToLower(str);
        if (lower == "screen") return PDFQuality::SCREEN;
        if (lower == "ebook") return PDFQuality::EBOOK;
        if (lower == "printer") return PDFQuality::PRINTER;
        if (lower == "prepress") return PDFQuality::PREPRESS;
        return PDFQuality::DEFAULT;
    }
    
    PaperSize StringToPaperSize(const std::string& str) const {
        std::string lower = ToLower(str);
        if (lower == "custom") return PaperSize::CUSTOM;
        if (lower == "letter") return PaperSize::LETTER;
        if (lower == "legal") return PaperSize::LEGAL;
        if (lower == "a3") return PaperSize::A3;
        if (lower == "a5") return PaperSize::A5;
        if (lower == "executive") return PaperSize::EXECUTIVE;
        return PaperSize::A4;
    }
    
    std::string GetHelpText() const {
        std::stringstream ss;
        ss << "PDF_LIB - A lightweight Ghostscript alternative\n\n";
        ss << "Usage: pdf_gs [options] [files]\n\n";
        ss << "Options:\n";
        
        for (const auto& param : parameters_) {
            ss << "  -" << param.short_name;
            if (param.type != ParameterType::BOOLEAN) {
                ss << "=<value>";
            }
            ss << "\n    " << param.description << "\n";
            if (!param.allowed_values.empty()) {
                ss << "    Allowed values: ";
                for (size_t i = 0; i < param.allowed_values.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << param.allowed_values[i];
                }
                ss << "\n";
            }
            ss << "\n";
        }
        
        return ss.str();
    }
    
    std::vector<ParameterDefinition> parameters_;
    std::vector<std::string> unrecognized_args_;
};

// CommandParser implementation
CommandParser::CommandParser() : pImpl(std::make_unique<Impl>()) {}

CommandParser::~CommandParser() = default;

bool CommandParser::ParseArguments(const std::vector<std::string>& arguments, 
                                  ProcessingOptions& options) {
    return pImpl->ParseArguments(arguments, options);
}

ErrorHandler& CommandParser::GetErrorHandler() {
    return ErrorHandler::GetInstance();
}

std::map<std::string, ParameterValue> CommandParser::GetParsedParameters() const {
    return std::map<std::string, ParameterValue>{}; // TODO: Implement
}

std::vector<std::string> CommandParser::GetUnrecognizedArguments() const {
    return pImpl->unrecognized_args_;
}

bool CommandParser::ValidateOptions(const ProcessingOptions& options) {
    // Basic validation
    if (options.input_files.empty() && options.device_name != "mswinpr2") {
        ErrorHandler::GetInstance().SetError(-1, "No input files specified");
        return false;
    }
    
    if (options.output_file.empty() && options.device_name != "mswinpr2") {
        ErrorHandler::GetInstance().SetError(-1, "No output file specified");
        return false;
    }
    
    return true;
}

std::string CommandParser::GetHelpText() const {
    return pImpl->GetHelpText();
}

std::vector<ParameterDefinition> CommandParser::GetParameterDefinitions() const {
    return pImpl->parameters_;
}

// Private method implementations
void CommandParser::InitializeParameterDefinitions() {
    pImpl->InitializeParameterDefinitions();
}

bool CommandParser::ParseArgument(const std::string& arg, int position, 
                                 std::map<std::string, ParameterValue>& params) {
    return pImpl->ParseArgument(arg, position, params);
}

bool CommandParser::ParseParameterValue(const ParameterDefinition& param_def,
                                       const std::string& value_str,
                                       ParameterValue& value) {
    return pImpl->ParseParameterValue(param_def, value_str, value);
}

bool CommandParser::ConvertToOptions(const std::map<std::string, ParameterValue>& params,
                                    ProcessingOptions& options) {
    return pImpl->ConvertToOptions(params, options);
}

const ParameterDefinition* CommandParser::FindParameterDefinition(const std::string& name) const {
    return pImpl->FindParameterDefinition(name);
}

bool CommandParser::StartsWith(const std::string& str, const std::string& prefix) const {
    return pImpl->StartsWith(str, prefix);
}

std::vector<std::string> CommandParser::SplitString(const std::string& str, char delimiter) const {
    return pImpl->SplitString(str, delimiter);
}

std::string CommandParser::TrimString(const std::string& str) const {
    return pImpl->TrimString(str);
}

std::string CommandParser::ToLower(const std::string& str) const {
    return pImpl->ToLower(str);
}

} // namespace PDFLib 