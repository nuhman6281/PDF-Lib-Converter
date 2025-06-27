/**
 * @file pdf_engine.h
 * @brief PDF engine for PDF_LIB
 */

#ifndef PDF_LIB_PDF_ENGINE_H
#define PDF_LIB_PDF_ENGINE_H

#include <string>
#include <vector>
#include <memory>
#include "../include/pdf_lib.h"
#include "error_handler.h"

namespace PDFLib {

// Forward declarations
class PostScriptInterpreter;

/**
 * @brief PDF page information
 */
struct PDFPageInfo {
    int page_number;           ///< Page number (1-based)
    double width_points;       ///< Page width in points
    double height_points;      ///< Page height in points
    double rotation;           ///< Page rotation in degrees
    std::string content_type;  ///< Content type (text, image, etc.)
};

/**
 * @brief PDF document information
 */
struct PDFDocumentInfo {
    std::string title;         ///< Document title
    std::string author;        ///< Document author
    std::string subject;       ///< Document subject
    std::string creator;       ///< Document creator
    std::string producer;      ///< Document producer
    std::string creation_date; ///< Creation date
    std::string modification_date; ///< Modification date
    int page_count;            ///< Total page count
    double version;            ///< PDF version
};

/**
 * @brief PDF engine class
 * 
 * Core PDF processing engine that handles PDF reading, writing,
 * and manipulation operations.
 */
class PDFEngine {
public:
    /**
     * @brief Constructor
     */
    PDFEngine();
    
    /**
     * @brief Destructor
     */
    ~PDFEngine();
    
    /**
     * @brief Initialize PDF engine
     * @param options Processing options
     * @param error_handler Error handler reference
     * @return true if initialization successful, false otherwise
     */
    bool Initialize(const ProcessingOptions& options, ErrorHandler& error_handler);
    
    /**
     * @brief Load PDF file
     * @param filepath File path to load
     * @param error_handler Error handler reference
     * @return true if loading successful, false otherwise
     */
    bool LoadFile(const std::string& filepath, ErrorHandler& error_handler);
    
    /**
     * @brief Load PostScript file
     * @param filepath File path to load
     * @param error_handler Error handler reference
     * @return true if loading successful, false otherwise
     */
    bool LoadPostScriptFile(const std::string& filepath, ErrorHandler& error_handler);
    
    /**
     * @brief Save PDF file
     * @param filepath Output file path
     * @param error_handler Error handler reference
     * @return true if saving successful, false otherwise
     */
    bool SaveFile(const std::string& filepath, ErrorHandler& error_handler);
    
    /**
     * @brief Get document information
     * @return Document information structure
     */
    PDFDocumentInfo GetDocumentInfo() const;
    
    /**
     * @brief Get page information
     * @param page_number Page number (1-based)
     * @return Page information structure
     */
    PDFPageInfo GetPageInfo(int page_number) const;
    
    /**
     * @brief Get total page count
     * @return Total number of pages
     */
    int GetPageCount() const;
    
    /**
     * @brief Apply PostScript transformations
     * @param interpreter PostScript interpreter reference
     * @param error_handler Error handler reference
     * @return true if application successful, false otherwise
     */
    bool ApplyTransformations(const PostScriptInterpreter& interpreter, ErrorHandler& error_handler);
    
    /**
     * @brief Set page offset
     * @param x X offset in points
     * @param y Y offset in points
     * @param error_handler Error handler reference
     * @return true if setting successful, false otherwise
     */
    bool SetPageOffset(double x, double y, ErrorHandler& error_handler);
    
    /**
     * @brief Set page margins
     * @param left Left margin in points
     * @param right Right margin in points
     * @param top Top margin in points
     * @param bottom Bottom margin in points
     * @param error_handler Error handler reference
     * @return true if setting successful, false otherwise
     */
    bool SetPageMargins(double left, double right, double top, double bottom, ErrorHandler& error_handler);
    
    /**
     * @brief Set page rotation
     * @param rotation Rotation in degrees
     * @param error_handler Error handler reference
     * @return true if setting successful, false otherwise
     */
    bool SetPageRotation(double rotation, ErrorHandler& error_handler);
    
    /**
     * @brief Set page size
     * @param width Width in points
     * @param height Height in points
     * @param error_handler Error handler reference
     * @return true if setting successful, false otherwise
     */
    bool SetPageSize(double width, double height, ErrorHandler& error_handler);
    
    /**
     * @brief Optimize PDF
     * @param quality Optimization quality
     * @param error_handler Error handler reference
     * @return true if optimization successful, false otherwise
     */
    bool OptimizePDF(PDFQuality quality, ErrorHandler& error_handler);
    
    /**
     * @brief Merge PDFs
     * @param filepaths Vector of PDF file paths to merge
     * @param error_handler Error handler reference
     * @return true if merging successful, false otherwise
     */
    bool MergePDFs(const std::vector<std::string>& filepaths, ErrorHandler& error_handler);
    
    /**
     * @brief Extract page
     * @param page_number Page number to extract (1-based)
     * @param output_filepath Output file path
     * @param error_handler Error handler reference
     * @return true if extraction successful, false otherwise
     */
    bool ExtractPage(int page_number, const std::string& output_filepath, ErrorHandler& error_handler);
    
    /**
     * @brief Delete page
     * @param page_number Page number to delete (1-based)
     * @param error_handler Error handler reference
     * @return true if deletion successful, false otherwise
     */
    bool DeletePage(int page_number, ErrorHandler& error_handler);
    
    /**
     * @brief Insert page
     * @param page_number Insert position (1-based)
     * @param source_filepath Source PDF file path
     * @param source_page_number Source page number (1-based)
     * @param error_handler Error handler reference
     * @return true if insertion successful, false otherwise
     */
    bool InsertPage(int page_number, const std::string& source_filepath, 
                   int source_page_number, ErrorHandler& error_handler);
    
    /**
     * @brief Get PDF data as buffer
     * @param data Output data buffer
     * @param size Output data size
     * @param error_handler Error handler reference
     * @return true if getting successful, false otherwise
     */
    bool GetPDFData(std::vector<uint8_t>& data, ErrorHandler& error_handler);
    
    /**
     * @brief Set PDF data from buffer
     * @param data Input data buffer
     * @param size Data size
     * @param error_handler Error handler reference
     * @return true if setting successful, false otherwise
     */
    bool SetPDFData(const uint8_t* data, size_t size, ErrorHandler& error_handler);
    
    /**
     * @brief Check if PDF is valid
     * @return true if PDF is valid, false otherwise
     */
    bool IsValid() const;
    
    /**
     * @brief Check if PDF is encrypted
     * @return true if PDF is encrypted, false otherwise
     */
    bool IsEncrypted() const;
    
    /**
     * @brief Clean up resources
     */
    void Cleanup();

private:
    /**
     * @brief Initialize PDFium (if available)
     * @param error_handler Error handler reference
     * @return true if initialization successful, false otherwise
     */
    bool InitializePDFium(ErrorHandler& error_handler);
    
    /**
     * @brief Load PDF using PDFium
     * @param filepath File path to load
     * @param error_handler Error handler reference
     * @return true if loading successful, false otherwise
     */
    bool LoadPDFWithPDFium(const std::string& filepath, ErrorHandler& error_handler);
    
    /**
     * @brief Load PDF using internal implementation
     * @param filepath File path to load
     * @param error_handler Error handler reference
     * @return true if loading successful, false otherwise
     */
    bool LoadPDFInternal(const std::string& filepath, ErrorHandler& error_handler);
    
    /**
     * @brief Save PDF using PDFium
     * @param filepath Output file path
     * @param error_handler Error handler reference
     * @return true if saving successful, false otherwise
     */
    bool SavePDFWithPDFium(const std::string& filepath, ErrorHandler& error_handler);
    
    /**
     * @brief Save PDF using internal implementation
     * @param filepath Output file path
     * @param error_handler Error handler reference
     * @return true if saving successful, false otherwise
     */
    bool SavePDFInternal(const std::string& filepath, ErrorHandler& error_handler);

    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace PDFLib

#endif // PDF_LIB_PDF_ENGINE_H 