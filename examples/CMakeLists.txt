# Examples CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# Example 1: Basic PDF processing
add_executable(basic_pdf_processor
    basic_pdf_processor.cpp
)

target_link_libraries(basic_pdf_processor pdf_lib)

# Example 2: Command-line interface
add_executable(cli_example
    cli_example.cpp
)

target_link_libraries(cli_example pdf_lib)

# Example 3: C API usage
add_executable(c_api_example
    c_api_example.cpp
)

target_link_libraries(c_api_example pdf_lib_c)

# Example 4: Windows printing
if(WIN32)
    add_executable(windows_printing_example
        windows_printing_example.cpp
    )
    
    target_link_libraries(windows_printing_example pdf_lib)
endif()

# Example 5: PostScript processing
add_executable(postscript_example
    postscript_example.cpp
)

target_link_libraries(postscript_example pdf_lib)

# Include directories
target_include_directories(basic_pdf_processor PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(cli_example PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(c_api_example PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(WIN32)
    target_include_directories(windows_printing_example PRIVATE ${CMAKE_SOURCE_DIR}/include)
endif()
target_include_directories(postscript_example PRIVATE ${CMAKE_SOURCE_DIR}/include) 