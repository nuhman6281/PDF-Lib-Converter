/**
 * @file command_line_parser.h
 * @brief Command-line parser for Ghostscript-style arguments
 * @version 1.0.0
 * @author PDF_LIB Team
 */

#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include <string>
#include <vector>
#include <map>
#include "../include/pdf_lib.h"

namespace PDFLib {

/**
 * @brief Command-line parser for Ghostscript-style arguments
 */
class CommandLineParser {
public:
    /**
     * @brief Constructor
     */
    CommandLineParser();
    
    /**
     * @brief Destructor
     */
    ~CommandLineParser();
    
    /**
     * @brief Parse command-line arguments
     * @param arguments Vector of command-line arguments
     * @param options Output processing options
     * @return true if parsing successful, false otherwise
     */
    bool Parse(const std::vector<std::string>& arguments, ProcessingOptions& options);
    
    /**
     * @brief Parse command-line arguments from argc/argv
     * @param argc Argument count
     * @param argv Argument vector
     * @param options Output processing options
     * @return true if parsing successful, false otherwise
     */
    bool Parse(int argc, char* argv[], ProcessingOptions& options);
    
    /**
     * @brief Get help text
     * @return Help text string
     */
    static std::string GetHelpText();
    
    /**
     * @brief Get version information
     * @return Version string
     */
    static std::string GetVersionText();
    
    /**
     * @brief Check if argument is a flag
     * @param arg Argument to check
     * @return true if flag, false otherwise
     */
    static bool IsFlag(const std::string& arg);
    
    /**
     * @brief Check if argument is a device specification
     * @param arg Argument to check
     * @return true if device, false otherwise
     */
    static bool IsDevice(const std::string& arg);

private:
    /**
     * @brief Parse a single argument
     * @param arg Argument to parse
     * @param options Processing options to update
     * @param next_arg Next argument (for parameters that take values)
     * @param has_next Whether there is a next argument
     * @return true if parsing successful, false otherwise
     */
    bool ParseArgument(const std::string& arg, ProcessingOptions& options, 
                       const std::string& next_arg, bool has_next);
    
    /**
     * @brief Parse quality setting
     * @param value Quality value string
     * @param options Processing options to update
     * @return true if parsing successful, false otherwise
     */
    bool ParseQuality(const std::string& value, ProcessingOptions& options);
    
    /**
     * @brief Parse paper size
     * @param value Paper size string
     * @param options Processing options to update
     * @return true if parsing successful, false otherwise
     */
    bool ParsePaperSize(const std::string& value, ProcessingOptions& options);
    
    /**
     * @brief Parse device name
     * @param value Device name string
     * @param options Processing options to update
     * @return true if parsing successful, false otherwise
     */
    bool ParseDevice(const std::string& value, ProcessingOptions& options);
    
    /**
     * @brief Parse margin values
     * @param value Margin string (e.g., "10,20,30,40")
     * @param options Processing options to update
     * @return true if parsing successful, false otherwise
     */
    bool ParseMargins(const std::string& value, ProcessingOptions& options);
    
    /**
     * @brief Parse page offset values
     * @param value Offset string (e.g., "10,20")
     * @param options Processing options to update
     * @return true if parsing successful, false otherwise
     */
    bool ParsePageOffset(const std::string& value, ProcessingOptions& options);
    
    /**
     * @brief Parse custom paper dimensions
     * @param value Dimensions string (e.g., "612,792")
     * @param options Processing options to update
     * @return true if parsing successful, false otherwise
     */
    bool ParseCustomDimensions(const std::string& value, ProcessingOptions& options);
    
    /**
     * @brief Validate processing options
     * @param options Options to validate
     * @return true if valid, false otherwise
     */
    bool ValidateOptions(const ProcessingOptions& options);
    
    // Known flags and their descriptions
    static const std::map<std::string, std::string> known_flags_;
    
    // Known devices and their descriptions
    static const std::map<std::string, std::string> known_devices_;
};

} // namespace PDFLib

#endif // COMMAND_LINE_PARSER_H 