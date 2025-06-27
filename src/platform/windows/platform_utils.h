/**
 * @file platform_utils.h
 * @brief Windows platform utilities header
 */

#ifndef PDF_LIB_WINDOWS_PLATFORM_UTILS_H
#define PDF_LIB_WINDOWS_PLATFORM_UTILS_H

#include <string>

namespace PDFLib {
namespace Platform {

bool InitializeWindowsPlatform();
void CleanupWindowsPlatform();
std::string GetWindowsVersion();
bool IsWindowsVistaOrLater();
bool IsWindows7OrLater();
bool IsWindows8OrLater();
bool IsWindows10OrLater();

} // namespace Platform
} // namespace PDFLib

#endif // PDF_LIB_WINDOWS_PLATFORM_UTILS_H 