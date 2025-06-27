/**
 * @file pdf_generator.h
 * @brief PDF generation backend for PDF_LIB
 */

#ifndef PDF_LIB_PDF_GENERATOR_H
#define PDF_LIB_PDF_GENERATOR_H

#include "postscript_parser.h"
#include "../include/pdf_lib.h"
#include <string>
#include <vector>
#include <memory>

namespace PDFLib {

// Forward declarations
class ErrorHandler;

/**
 * @brief PDF page information for generator
 */
struct PDFPageData {
    double width = 595.276;  // A4 width in points
    double height = 841.890; // A4 height in points
    std::vector<PathElement> paths;
    std::vector<TextElement> text_elements;
};

/**
 * @brief PDF document metadata
 */
struct PDFMetadata {
    std::string title;
    std::string author;
    std::string subject;
    std::string keywords;
    std::string creator = "PDF_LIB";
    std::string producer = "PDF_LIB PostScript to PDF Converter";
    double compatibility_level = 1.7;
};

/**
 * @brief PDF generator class
 */
class PDFGenerator {
public:
    PDFGenerator();
    ~PDFGenerator();
    
    /**
     * @brief Initialize PDF generator
     * @param options Processing options
     * @param error_handler Error handler instance
     * @return true if initialization successful
     */
    bool Initialize(const ProcessingOptions& options, ErrorHandler& error_handler);
    
    /**
     * @brief Create PDF from PostScript parser data
     * @param parser PostScript parser with parsed data
     * @param output_file Output PDF file path
     * @param error_handler Error handler instance
     * @return true if PDF creation successful
     */
    bool CreatePDF(const PostScriptParser& parser, const std::string& output_file, 
                   ErrorHandler& error_handler);
    
    /**
     * @brief Add page to PDF
     * @param page_data Page information
     * @return true if page added successfully
     */
    bool AddPage(const PDFPageData& page_data);
    
    /**
     * @brief Set PDF metadata
     * @param metadata PDF metadata
     */
    void SetMetadata(const PDFMetadata& metadata);
    
    /**
     * @brief Write PDF to file
     * @param filepath Output file path
     * @param error_handler Error handler instance
     * @return true if write successful
     */
    bool WritePDF(const std::string& filepath, ErrorHandler& error_handler);
    
    /**
     * @brief Get generated PDF size
     * @return PDF size in bytes
     */
    size_t GetPDFSize() const;
    
    /**
     * @brief Clear all pages and reset
     */
    void Clear();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace PDFLib

#endif // PDF_LIB_PDF_GENERATOR_H 