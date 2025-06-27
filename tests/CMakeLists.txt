# Tests CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# Find GTest
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    message(STATUS "GTest not found, skipping tests")
    return()
endif()

# Test executable
add_executable(pdf_lib_tests
    test_pdf_processor.cpp
    test_command_parser.cpp
    test_device_manager.cpp
    test_error_handler.cpp
    test_utils.cpp
    test_pdf_engine.cpp
    test_postscript_interpreter.cpp
    test_resource_manager.cpp
)

# Link libraries
target_link_libraries(pdf_lib_tests
    pdf_lib
    GTest::gtest
    GTest::gtest_main
)

# Include directories
target_include_directories(pdf_lib_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Add tests
add_test(NAME PDFLibTests COMMAND pdf_lib_tests)

# Set test properties
set_tests_properties(PDFLibTests PROPERTIES
    TIMEOUT 300
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
) 