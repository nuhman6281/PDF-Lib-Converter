/**
 * @file string_utils.h
 * @brief String utility functions for PDF_LIB
 */

#ifndef PDF_LIB_STRING_UTILS_H
#define PDF_LIB_STRING_UTILS_H

#include <string>
#include <vector>

namespace PDFLib {
namespace Utils {

/**
 * @brief Convert string to lowercase
 * @param str String to convert
 * @return Lowercase string
 */
std::string ToLower(const std::string& str);

/**
 * @brief Convert string to uppercase
 * @param str String to convert
 * @return Uppercase string
 */
std::string ToUpper(const std::string& str);

/**
 * @brief Trim whitespace from string
 * @param str String to trim
 * @return Trimmed string
 */
std::string Trim(const std::string& str);

/**
 * @brief Split string by delimiter
 * @param str String to split
 * @param delimiter Delimiter character
 * @return Vector of split strings
 */
std::vector<std::string> Split(const std::string& str, char delimiter);

/**
 * @brief Replace all occurrences of substring
 * @param str Original string
 * @param from Substring to replace
 * @param to Replacement substring
 * @return String with replacements
 */
std::string Replace(const std::string& str, const std::string& from, const std::string& to);

/**
 * @brief Check if string starts with prefix
 * @param str String to check
 * @param prefix Prefix to check for
 * @return true if string starts with prefix, false otherwise
 */
bool StartsWith(const std::string& str, const std::string& prefix);

/**
 * @brief Convert string to integer
 * @param str String to convert
 * @param value Output integer value
 * @return true if conversion successful, false otherwise
 */
bool StringToInt(const std::string& str, int& value);

/**
 * @brief Convert string to double
 * @param str String to convert
 * @param value Output double value
 * @return true if conversion successful, false otherwise
 */
bool StringToDouble(const std::string& str, double& value);

/**
 * @brief Check if string is a number
 * @param str String to check
 * @return true if string is a number, false otherwise
 */
bool IsNumber(const std::string& str);

} // namespace Utils
} // namespace PDFLib

#endif // PDF_LIB_STRING_UTILS_H 