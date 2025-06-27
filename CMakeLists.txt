cmake_minimum_required(VERSION 3.16)
project(PDF_LIB VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(ENABLE_WINDOWS_PRINTER "Enable Windows printer support" ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Platform-specific settings
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
    set(PLATFORM_SOURCES src/platform/windows)
else()
    set(PLATFORM_SOURCES src/platform/linux)
endif()

# Find required packages
find_package(PkgConfig QUIET)

# PDFium (if available)
find_path(PDFIUM_INCLUDE_DIR pdfium/fpdfview.h
    PATHS
    /usr/include
    /usr/local/include
    /opt/include
    $ENV{PDFIUM_ROOT}/include
)

find_library(PDFIUM_LIBRARY
    NAMES pdfium
    PATHS
    /usr/lib
    /usr/local/lib
    /opt/lib
    $ENV{PDFIUM_ROOT}/lib
)

if(PDFIUM_INCLUDE_DIR AND PDFIUM_LIBRARY)
    set(HAVE_PDFIUM TRUE)
    message(STATUS "Found PDFium: ${PDFIUM_LIBRARY}")
else()
    message(STATUS "PDFium not found, will use internal PDF implementation")
endif()

# Windows-specific libraries
if(WIN32)
    set(WINDOWS_LIBS
        gdi32
        gdiplus
        winspool
        kernel32
        user32
        advapi32
        shell32
    )
endif()

# Source files
set(CORE_SOURCES
    src/core/pdf_processor.cpp
    src/core/command_parser.cpp
    src/core/device_manager.cpp
    src/core/postscript_interpreter.cpp
    src/core/resource_manager.cpp
    src/core/error_handler.cpp
    src/core/pdf_engine.cpp
)

set(DEVICE_SOURCES
    src/devices/pdf_write_device.cpp
    src/devices/mswin_printer_device.cpp
    src/devices/postscript_processor.cpp
)

set(PARSER_SOURCES
    src/parsers/command_line_parser.cpp
    src/parsers/postscript_parser.cpp
)

set(UTILS_SOURCES
    src/utils/file_utils.cpp
    src/utils/string_utils.cpp
    src/utils/memory_utils.cpp
)

set(PLATFORM_SOURCES
    ${PLATFORM_SOURCES}/platform_utils.cpp
)

if(WIN32)
    list(APPEND PLATFORM_SOURCES ${PLATFORM_SOURCES}/windows_printer.cpp)
endif()

# Main library
add_library(pdf_lib ${CORE_SOURCES} ${DEVICE_SOURCES} ${PARSER_SOURCES} ${UTILS_SOURCES} ${PLATFORM_SOURCES})

# Set library properties
set_target_properties(pdf_lib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER include/pdf_lib.h
)

# Link libraries
target_link_libraries(pdf_lib ${WINDOWS_LIBS})

if(HAVE_PDFIUM)
    target_include_directories(pdf_lib PRIVATE ${PDFIUM_INCLUDE_DIR})
    target_link_libraries(pdf_lib ${PDFIUM_LIBRARY})
endif()

# C API wrapper
add_library(pdf_lib_c src/api/c_api.cpp)
target_link_libraries(pdf_lib_c pdf_lib)
set_target_properties(pdf_lib_c PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER include/pdf_lib_c.h
)

# Command-line executable
add_executable(pdf_gs src/cli/main.cpp)
target_link_libraries(pdf_gs pdf_lib)

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Installation
install(TARGETS pdf_lib pdf_lib_c pdf_gs
    EXPORT PDFLibTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)

# Export targets
install(EXPORT PDFLibTargets
    FILE PDFLibTargets.cmake
    NAMESPACE PDFLib::
    DESTINATION lib/cmake/PDFLib
)

# Create config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    PDFLibConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/PDFLibConfigVersion.cmake
    DESTINATION lib/cmake/PDFLib
) 