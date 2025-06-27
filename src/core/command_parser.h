/**
 * @file command_parser.h
 * @brief Command-line argument parser for PDF_LIB
 */

#ifndef PDF_LIB_COMMAND_PARSER_H
#define PDF_LIB_COMMAND_PARSER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../include/pdf_lib.h"
#include "error_handler.h"

namespace PDFLib {

/**
 * @brief Command parameter types
 */
enum class ParameterType {
    BOOLEAN,        ///< Boolean flag (true/false)
    STRING,         ///< String value
    INTEGER,        ///< Integer value
    DOUBLE,         ///< Double value
    QUALITY,        ///< PDF quality setting
    PAPER_SIZE,     ///< Paper size
    DEVICE_TYPE     ///< Device type
};

/**
 * @brief Command parameter definition
 */
struct ParameterDefinition {
    std::string name;           ///< Parameter name
    std::string short_name;     ///< Short parameter name
    ParameterType type;         ///< Parameter type
    std::string description;    ///< Parameter description
    std::string default_value;  ///< Default value
    bool required;              ///< Whether parameter is required
    std::vector<std::string> allowed_values; ///< Allowed values for enum types
};

/**
 * @brief Parsed parameter value
 */
struct ParameterValue {
    std::string name;           ///< Parameter name
    std::string value;          ///< Parameter value
    bool is_set;                ///< Whether parameter was set
    int position;               ///< Position in command line
};

/**
 * @brief Command parser class
 * 
 * Parses Ghostscript-style command-line arguments and converts them
 * to ProcessingOptions structure.
 */
class CommandParser {
public:
    /**
     * @brief Constructor
     */
    CommandParser();
    
    /**
     * @brief Destructor
     */
    ~CommandParser();
    
    /**
     * @brief Parse command-line arguments
     * @param arguments Command-line arguments
     * @param options Output processing options
     * @return true if parsing successful, false otherwise
     */
    bool ParseArguments(const std::vector<std::string>& arguments, 
                       ProcessingOptions& options);
    
    /**
     * @brief Get error handler
     * @return Reference to error handler
     */
    ErrorHandler& GetErrorHandler();
    
    /**
     * @brief Get parsed parameters
     * @return Map of parsed parameters
     */
    std::map<std::string, ParameterValue> GetParsedParameters() const;
    
    /**
     * @brief Get unrecognized arguments
     * @return Vector of unrecognized arguments
     */
    std::vector<std::string> GetUnrecognizedArguments() const;
    
    /**
     * @brief Validate parsed options
     * @param options Processing options to validate
     * @return true if valid, false otherwise
     */
    bool ValidateOptions(const ProcessingOptions& options);
    
    /**
     * @brief Get help text
     * @return Help text string
     */
    std::string GetHelpText() const;
    
    /**
     * @brief Get parameter definitions
     * @return Vector of parameter definitions
     */
    std::vector<ParameterDefinition> GetParameterDefinitions() const;

private:
    /**
     * @brief Initialize parameter definitions
     */
    void InitializeParameterDefinitions();
    
    /**
     * @brief Parse single argument
     * @param arg Argument string
     * @param position Argument position
     * @param params Output parameter map
     * @return true if parsing successful, false otherwise
     */
    bool ParseArgument(const std::string& arg, int position, 
                      std::map<std::string, ParameterValue>& params);
    
    /**
     * @brief Parse parameter value
     * @param param_def Parameter definition
     * @param value_str Value string
     * @param value Output parameter value
     * @return true if parsing successful, false otherwise
     */
    bool ParseParameterValue(const ParameterDefinition& param_def,
                           const std::string& value_str,
                           ParameterValue& value);
    
    /**
     * @brief Convert parameters to ProcessingOptions
     * @param params Parameter map
     * @param options Output processing options
     * @return true if conversion successful, false otherwise
     */
    bool ConvertToOptions(const std::map<std::string, ParameterValue>& params,
                         ProcessingOptions& options);
    
    /**
     * @brief Find parameter definition by name
     * @param name Parameter name
     * @return Parameter definition or nullptr if not found
     */
    const ParameterDefinition* FindParameterDefinition(const std::string& name) const;
    
    /**
     * @brief Check if string starts with prefix
     * @param str String to check
     * @param prefix Prefix to check for
     * @return true if string starts with prefix, false otherwise
     */
    bool StartsWith(const std::string& str, const std::string& prefix) const;
    
    /**
     * @brief Split string by delimiter
     * @param str String to split
     * @param delimiter Delimiter character
     * @return Vector of split strings
     */
    std::vector<std::string> SplitString(const std::string& str, char delimiter) const;
    
    /**
     * @brief Trim whitespace from string
     * @param str String to trim
     * @return Trimmed string
     */
    std::string TrimString(const std::string& str) const;
    
    /**
     * @brief Convert string to lowercase
     * @param str String to convert
     * @return Lowercase string
     */
    std::string ToLower(const std::string& str) const;

    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace PDFLib

#endif // PDF_LIB_COMMAND_PARSER_H 