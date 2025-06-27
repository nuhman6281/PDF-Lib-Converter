/**
 * @file postscript_interpreter.h
 * @brief PostScript interpreter for PDF_LIB
 */

#ifndef PDF_LIB_POSTSCRIPT_INTERPRETER_H
#define PDF_LIB_POSTSCRIPT_INTERPRETER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "error_handler.h"

namespace PDFLib {

/**
 * @brief PostScript transformation types
 */
enum class PostScriptTransformType {
    PAGE_OFFSET,    ///< Page offset transformation
    MARGINS,        ///< Margins transformation
    ROTATION,       ///< Page rotation
    SCALING         ///< Page scaling
};

/**
 * @brief PostScript transformation data
 */
struct PostScriptTransform {
    PostScriptTransformType type;
    double x;       ///< X coordinate or first parameter
    double y;       ///< Y coordinate or second parameter
    std::string additional_data; ///< Additional transformation data
};

/**
 * @brief PostScript interpreter class
 * 
 * Interprets and executes PostScript commands, particularly those
 * related to page transformations and device settings.
 */
class PostScriptInterpreter {
public:
    /**
     * @brief Constructor
     */
    PostScriptInterpreter();
    
    /**
     * @brief Destructor
     */
    ~PostScriptInterpreter();
    
    /**
     * @brief Initialize interpreter
     * @param error_handler Error handler reference
     * @return true if initialization successful, false otherwise
     */
    bool Initialize(ErrorHandler& error_handler);
    
    /**
     * @brief Execute PostScript command
     * @param command PostScript command string
     * @param error_handler Error handler reference
     * @return true if execution successful, false otherwise
     */
    bool ExecuteCommand(const std::string& command, ErrorHandler& error_handler);
    
    /**
     * @brief Execute multiple PostScript commands
     * @param commands Vector of PostScript commands
     * @param error_handler Error handler reference
     * @return true if execution successful, false otherwise
     */
    bool ExecuteCommands(const std::vector<std::string>& commands, ErrorHandler& error_handler);
    
    /**
     * @brief Check if there are any transformations
     * @return true if transformations exist, false otherwise
     */
    bool HasTransformations() const;
    
    /**
     * @brief Get all transformations
     * @return Vector of transformations
     */
    std::vector<PostScriptTransform> GetTransformations() const;
    
    /**
     * @brief Get page offset transformation
     * @param x Output X offset
     * @param y Output Y offset
     * @return true if page offset exists, false otherwise
     */
    bool GetPageOffset(double& x, double& y) const;
    
    /**
     * @brief Get margins transformation
     * @param left Output left margin
     * @param right Output right margin
     * @param top Output top margin
     * @param bottom Output bottom margin
     * @return true if margins exist, false otherwise
     */
    bool GetMargins(double& left, double& right, double& top, double& bottom) const;
    
    /**
     * @brief Clear all transformations
     */
    void ClearTransformations();
    
    /**
     * @brief Clean up resources
     */
    void Cleanup();

private:
    /**
     * @brief Parse PostScript command
     * @param command Command string
     * @param tokens Output token vector
     * @return true if parsing successful, false otherwise
     */
    bool ParseCommand(const std::string& command, std::vector<std::string>& tokens);
    
    /**
     * @brief Execute setpagedevice command
     * @param tokens Command tokens
     * @param error_handler Error handler reference
     * @return true if execution successful, false otherwise
     */
    bool ExecuteSetPageDevice(const std::vector<std::string>& tokens, ErrorHandler& error_handler);
    
    /**
     * @brief Parse PostScript dictionary
     * @param tokens Command tokens
     * @param start_index Starting token index
     * @param dict Output dictionary map
     * @return true if parsing successful, false otherwise
     */
    bool ParseDictionary(const std::vector<std::string>& tokens, size_t start_index,
                        std::map<std::string, std::string>& dict);
    
    /**
     * @brief Parse PostScript array
     * @param tokens Command tokens
     * @param start_index Starting token index
     * @param array Output array
     * @return true if parsing successful, false otherwise
     */
    bool ParseArray(const std::vector<std::string>& tokens, size_t start_index,
                   std::vector<double>& array);
    
    /**
     * @brief Tokenize PostScript string
     * @param str Input string
     * @param tokens Output tokens
     * @return true if tokenization successful, false otherwise
     */
    bool Tokenize(const std::string& str, std::vector<std::string>& tokens);
    
    /**
     * @brief Check if string is a number
     * @param str String to check
     * @return true if string is a number, false otherwise
     */
    bool IsNumber(const std::string& str) const;
    
    /**
     * @brief Convert string to double
     * @param str String to convert
     * @param value Output value
     * @return true if conversion successful, false otherwise
     */
    bool StringToDouble(const std::string& str, double& value) const;
    
    /**
     * @brief Trim whitespace from string
     * @param str String to trim
     * @return Trimmed string
     */
    std::string TrimString(const std::string& str) const;

    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace PDFLib

#endif // PDF_LIB_POSTSCRIPT_INTERPRETER_H 