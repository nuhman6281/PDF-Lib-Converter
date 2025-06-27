/**
 * @file platform_utils.cpp
 * @brief Windows platform utilities implementation (stub)
 */

#include "platform_utils.h"

namespace PDFLib {
namespace Platform {

bool InitializeWindowsPlatform() {
    // Stub implementation
    return true;
}

void CleanupWindowsPlatform() {
    // Stub implementation
}

std::string GetWindowsVersion() {
    return "Windows (stub)";
}

bool IsWindowsVistaOrLater() {
    return true; // Stub implementation
}

bool IsWindows7OrLater() {
    return true; // Stub implementation
}

bool IsWindows8OrLater() {
    return true; // Stub implementation
}

bool IsWindows10OrLater() {
    return true; // Stub implementation
}

} // namespace Platform
} // namespace PDFLib 