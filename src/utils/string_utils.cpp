/**
 * @file string_utils.cpp
 * @brief String utility functions implementation
 */

#include "string_utils.h"
#include <algorithm>
#include <sstream>
#include <cctype>
#include <cstring>

namespace PDFLib {
namespace Utils {

std::string ToLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string ToUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string Trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> Split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(Trim(item));
    }
    
    return result;
}

std::string Replace(const std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) {
        return str;
    }
    
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    
    return result;
}

bool StartsWith(const std::string& str, const std::string& prefix) {
    if (prefix.length() > str.length()) {
        return false;
    }
    return str.compare(0, prefix.length(), prefix) == 0;
}

bool StringToInt(const std::string& str, int& value) {
    try {
        value = std::stoi(str);
        return true;
    } catch (...) {
        return false;
    }
}

bool StringToDouble(const std::string& str, double& value) {
    try {
        value = std::stod(str);
        return true;
    } catch (...) {
        return false;
    }
}

bool IsNumber(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    
    std::string trimmed = Trim(str);
    if (trimmed.empty()) {
        return false;
    }
    
    // Check for sign
    size_t start = 0;
    if (trimmed[0] == '+' || trimmed[0] == '-') {
        start = 1;
        if (trimmed.length() == 1) {
            return false;
        }
    }
    
    // Check for decimal point
    bool has_decimal = false;
    bool has_digit = false;
    
    for (size_t i = start; i < trimmed.length(); ++i) {
        if (trimmed[i] == '.') {
            if (has_decimal) {
                return false; // Multiple decimal points
            }
            has_decimal = true;
        } else if (std::isdigit(trimmed[i])) {
            has_digit = true;
        } else {
            return false; // Invalid character
        }
    }
    
    return has_digit;
}

} // namespace Utils
} // namespace PDFLib 