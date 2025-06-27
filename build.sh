#!/bin/bash

# PDF_LIB Build Script
# This script simplifies the build process for Unix-like systems

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    print_error "CMakeLists.txt not found. Please run this script from the PDF_LIB root directory."
    exit 1
fi

# Parse command line arguments
BUILD_TYPE="Release"
BUILD_TESTS="ON"
BUILD_EXAMPLES="ON"
CLEAN_BUILD=false
INSTALL=false
INSTALL_PREFIX="/usr/local"

while [[ $# -gt 0 ]]; do
    case $1 in
    --debug)
        BUILD_TYPE="Debug"
        shift
        ;;
    --release)
        BUILD_TYPE="Release"
        shift
        ;;
    --no-tests)
        BUILD_TESTS="OFF"
        shift
        ;;
    --no-examples)
        BUILD_EXAMPLES="OFF"
        shift
        ;;
    --clean)
        CLEAN_BUILD=true
        shift
        ;;
    --install)
        INSTALL=true
        shift
        ;;
    --prefix)
        INSTALL_PREFIX="$2"
        shift 2
        ;;
    --help)
        echo "PDF_LIB Build Script"
        echo ""
        echo "Usage: $0 [options]"
        echo ""
        echo "Options:"
        echo "  --debug          Build in Debug mode"
        echo "  --release        Build in Release mode (default)"
        echo "  --no-tests       Skip building tests"
        echo "  --no-examples    Skip building examples"
        echo "  --clean          Clean build directory before building"
        echo "  --install        Install after building"
        echo "  --prefix DIR     Installation prefix (default: /usr/local)"
        echo "  --help           Show this help message"
        echo ""
        exit 0
        ;;
    *)
        print_error "Unknown option: $1"
        echo "Use --help for usage information"
        exit 1
        ;;
    esac
done

print_status "Building PDF_LIB in $BUILD_TYPE mode"

# Check for required tools
if ! command -v cmake &>/dev/null; then
    print_error "CMake is required but not installed."
    exit 1
fi

if ! command -v make &>/dev/null && ! command -v ninja &>/dev/null; then
    print_error "Either make or ninja is required but neither is installed."
    exit 1
fi

# Use ninja if available, otherwise use make
if command -v ninja &>/dev/null; then
    GENERATOR="Ninja"
    BUILD_TOOL="ninja"
else
    GENERATOR="Unix Makefiles"
    BUILD_TOOL="make"
fi

print_status "Using generator: $GENERATOR"

# Create build directory
BUILD_DIR="build"
if [ "$CLEAN_BUILD" = true ]; then
    print_status "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
print_status "Configuring with CMake..."
cmake .. \
    -G "$GENERATOR" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DBUILD_TESTS="$BUILD_TESTS" \
    -DBUILD_EXAMPLES="$BUILD_EXAMPLES" \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX"

if [ $? -ne 0 ]; then
    print_error "CMake configuration failed"
    exit 1
fi

# Build the project
print_status "Building PDF_LIB..."
$BUILD_TOOL

if [ $? -ne 0 ]; then
    print_error "Build failed"
    exit 1
fi

print_success "Build completed successfully!"

# Run tests if built
if [ "$BUILD_TESTS" = "ON" ]; then
    if [ -f "pdf_lib_tests" ] || [ -f "bin/pdf_lib_tests" ]; then
        print_status "Running tests..."
        if [ -f "pdf_lib_tests" ]; then
            ./pdf_lib_tests
        else
            ./bin/pdf_lib_tests
        fi

        if [ $? -eq 0 ]; then
            print_success "All tests passed!"
        else
            print_warning "Some tests failed"
        fi
    fi
fi

# Install if requested
if [ "$INSTALL" = true ]; then
    print_status "Installing PDF_LIB to $INSTALL_PREFIX..."
    $BUILD_TOOL install

    if [ $? -eq 0 ]; then
        print_success "Installation completed successfully!"
    else
        print_error "Installation failed"
        exit 1
    fi
fi

print_success "PDF_LIB build process completed!"
print_status "Build artifacts are in the $BUILD_DIR directory"

# Show what was built
echo ""
print_status "Built components:"
if [ -f "libpdf_lib.a" ] || [ -f "libpdf_lib.so" ] || [ -f "pdf_lib.lib" ] || [ -f "pdf_lib.dll" ]; then
    echo "  ✓ PDF_LIB library"
fi
if [ -f "libpdf_lib_c.a" ] || [ -f "libpdf_lib_c.so" ] || [ -f "pdf_lib_c.lib" ] || [ -f "pdf_lib_c.dll" ]; then
    echo "  ✓ PDF_LIB C API library"
fi
if [ -f "pdf_gs" ] || [ -f "pdf_gs.exe" ]; then
    echo "  ✓ Command-line interface (pdf_gs)"
fi
if [ -f "basic_pdf_processor" ] || [ -f "basic_pdf_processor.exe" ]; then
    echo "  ✓ Basic PDF processor example"
fi
if [ -f "c_api_example" ] || [ -f "c_api_example.exe" ]; then
    echo "  ✓ C API example"
fi
if [ -f "pdf_lib_tests" ] || [ -f "bin/pdf_lib_tests" ]; then
    echo "  ✓ Test suite"
fi

echo ""
print_status "Next steps:"
echo "  - Run examples: ./build/bin/basic_pdf_processor"
echo "  - Test CLI: ./build/bin/pdf_gs --help"
echo "  - Run tests: ./build/bin/pdf_lib_tests"
