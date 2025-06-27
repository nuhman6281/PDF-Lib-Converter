/**
 * @file pdf_generator.cpp
 * @brief PDF generation backend implementation
 */

#include "pdf_generator.h"
#include "error_handler.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

namespace PDFLib {

class PDFGenerator::Impl {
public:
    std::vector<PDFPageData> pages_;
    PDFMetadata metadata_;
    ProcessingOptions options_;
    
    struct PDFObject {
        int id;
        size_t offset;
        std::string content;
    };
    
    std::vector<PDFObject> objects_;
    int next_object_id_ = 1;
    
    bool Initialize(const ProcessingOptions& options, ErrorHandler& error_handler) {
        options_ = options;
        metadata_.compatibility_level = options.compatibility_level;
        
        // Clear any previous data
        pages_.clear();
        objects_.clear();
        next_object_id_ = 1;
        
        error_handler.Log("PDF generator initialized", ErrorSeverity::INFO);
        return true;
    }
    
    bool CreatePDF(const PostScriptParser& parser, const std::string& output_file, 
                   ErrorHandler& error_handler) {
        try {
            // Clear previous data
            pages_.clear();
            
            // Convert PostScript pages to PDF pages
            int page_count = parser.GetPageCount();
            for (int i = 0; i < page_count; ++i) {
                PDFPageData page_data;
                
                if (!parser.GetPageDimensions(i, page_data.width, page_data.height)) {
                    // Use default A4 dimensions
                    page_data.width = 595.276;
                    page_data.height = 841.890;
                }
                
                page_data.paths = parser.GetPagePaths(i);
                page_data.text_elements = parser.GetPageText(i);
                
                pages_.push_back(page_data);
            }
            
            // Set metadata from parser if available
            if (!parser.GetTitle().empty()) {
                metadata_.title = parser.GetTitle();
            }
            if (!parser.GetCreator().empty()) {
                metadata_.creator = parser.GetCreator();
            }
            
            // Generate and write PDF
            if (!WritePDF(output_file, error_handler)) {
                return false;
            }
            
            error_handler.Log("PDF created successfully: " + output_file, ErrorSeverity::INFO);
            return true;
            
        } catch (const std::exception& e) {
            error_handler.SetError(-3, "PDF creation error: " + std::string(e.what()));
            return false;
        }
    }
    
    bool WritePDF(const std::string& filepath, ErrorHandler& error_handler) {
        try {
            std::ofstream file(filepath, std::ios::binary);
            if (!file.is_open()) {
                error_handler.SetError(-4, "Cannot create PDF file: " + filepath);
                return false;
            }
            
            std::ostringstream pdf_content;
            
            // Write PDF header
            WritePDFHeader(pdf_content);
            
            // Create PDF objects
            CreateCatalogObject();
            CreatePagesObject();
            CreatePageObjects();
            CreateFontObject();
            
            // Write objects and build cross-reference table
            std::vector<size_t> xref_offsets;
            for (auto& obj : objects_) {
                obj.offset = pdf_content.tellp();
                xref_offsets.push_back(obj.offset);
                
                pdf_content << obj.id << " 0 obj\n";
                pdf_content << obj.content;
                pdf_content << "endobj\n\n";
            }
            
            // Write cross-reference table
            size_t xref_offset = pdf_content.tellp();
            WriteCrossReferenceTable(pdf_content, xref_offsets);
            
            // Write trailer
            WriteTrailer(pdf_content, xref_offset);
            
            // Write to file
            file << pdf_content.str();
            file.close();
            
            return true;
            
        } catch (const std::exception& e) {
            error_handler.SetError(-5, "Error writing PDF file: " + std::string(e.what()));
            return false;
        }
    }
    
private:
    void WritePDFHeader(std::ostringstream& content) {
        content << "%PDF-" << std::fixed << std::setprecision(1) << metadata_.compatibility_level << "\n";
        content << "%\xE2\xE3\xCF\xD3\n"; // Binary comment to indicate binary content
    }
    
    void CreateCatalogObject() {
        PDFObject catalog;
        catalog.id = next_object_id_++;
        
        std::ostringstream content;
        content << "<<\n";
        content << "/Type /Catalog\n";
        content << "/Pages " << next_object_id_ << " 0 R\n";
        content << ">>\n";
        
        catalog.content = content.str();
        objects_.push_back(catalog);
    }
    
    void CreatePagesObject() {
        PDFObject pages_obj;
        pages_obj.id = next_object_id_++;
        
        std::ostringstream content;
        content << "<<\n";
        content << "/Type /Pages\n";
        content << "/Count " << pages_.size() << "\n";
        content << "/Kids [";
        
        // Reference to page objects (will be created next)
        for (size_t i = 0; i < pages_.size(); ++i) {
            content << next_object_id_ + static_cast<int>(i) << " 0 R ";
        }
        content << "]\n";
        content << ">>\n";
        
        pages_obj.content = content.str();
        objects_.push_back(pages_obj);
    }
    
    void CreatePageObjects() {
        // Calculate font object ID first
        int font_obj_id = next_object_id_ + static_cast<int>(pages_.size() * 2);
        
        for (size_t page_idx = 0; page_idx < pages_.size(); ++page_idx) {
            const auto& page = pages_[page_idx];
            
            // Create page content stream
            std::ostringstream page_stream;
            GeneratePageContent(page, page_stream);
            
            // Create page object first
            PDFObject page_obj;
            page_obj.id = next_object_id_++;
            
            // Create content stream object with next ID
            PDFObject content_obj;
            content_obj.id = next_object_id_++;
            
            std::ostringstream content_obj_stream;
            content_obj_stream << "<<\n";
            content_obj_stream << "/Length " << page_stream.str().length() << "\n";
            content_obj_stream << ">>\n";
            content_obj_stream << "stream\n";
            content_obj_stream << page_stream.str();
            content_obj_stream << "\nendstream\n";
            
            content_obj.content = content_obj_stream.str();
            
            std::ostringstream page_obj_content;
            page_obj_content << "<<\n";
            page_obj_content << "/Type /Page\n";
            page_obj_content << "/Parent " << objects_[1].id << " 0 R\n"; // Reference to Pages object
            page_obj_content << "/MediaBox [0 0 " << page.width << " " << page.height << "]\n";
            page_obj_content << "/Contents " << content_obj.id << " 0 R\n";
            page_obj_content << "/Resources <<\n";
            page_obj_content << "  /Font << /F1 " << font_obj_id << " 0 R >>\n";
            page_obj_content << ">>\n";
            page_obj_content << ">>\n";
            
            page_obj.content = page_obj_content.str();
            objects_.push_back(page_obj);
            objects_.push_back(content_obj);
        }
    }
    
    void CreateFontObject() {
        PDFObject font_obj;
        font_obj.id = next_object_id_++;
        
        std::ostringstream content;
        content << "<<\n";
        content << "/Type /Font\n";
        content << "/Subtype /Type1\n";
        content << "/BaseFont /Helvetica\n";
        content << ">>\n";
        
        font_obj.content = content.str();
        objects_.push_back(font_obj);
    }
    
    void GeneratePageContent(const PDFPageData& page, std::ostringstream& content) {
        content << std::fixed << std::setprecision(2);
        
        // Set initial graphics state
        content << "q\n"; // Save graphics state
        content << "1 0 0 1 0 0 cm\n"; // Identity matrix
        
        // Set default stroke color to black and line width for visibility
        content << "0 0 0 RG\n"; // Set stroke color to black (RGB)
        content << "0 0 0 rg\n"; // Set fill color to black (RGB)  
        content << "1 w\n"; // Set line width to 1 point
        content << "1 J\n"; // Set line cap to round
        content << "1 j\n"; // Set line join to round
        
        // Add debug: draw a simple test rectangle to verify PDF structure
        content << "% Test rectangle\n";
        content << "100 100 m\n";
        content << "150 100 l\n";
        content << "150 150 l\n";
        content << "100 150 l\n";
        content << "h\n";
        content << "S\n";
        
        // Render paths first (background graphics)
        bool has_open_path = false;
        for (const auto& path : page.paths) {
            switch (path.type) {
                case PathElement::MOVE_TO:
                    if (path.points.size() >= 2) {
                        if (has_open_path) {
                            content << "S\n"; // Stroke previous path
                            has_open_path = false;
                        }
                        content << path.points[0] << " " << path.points[1] << " m\n";
                        has_open_path = true;
                    }
                    break;
                    
                case PathElement::LINE_TO:
                    if (path.points.size() >= 2) {
                        content << path.points[0] << " " << path.points[1] << " l\n";
                    }
                    break;
                    
                case PathElement::CURVE_TO:
                    if (path.points.size() >= 6) {
                        content << path.points[0] << " " << path.points[1] << " "
                               << path.points[2] << " " << path.points[3] << " "
                               << path.points[4] << " " << path.points[5] << " c\n";
                    }
                    break;
                    
                case PathElement::CLOSE_PATH:
                    content << "h\n";
                    break;
            }
        }
        
        if (has_open_path) {
            content << "S\n"; // Stroke final path
        }
        
        // Render text elements
        if (!page.text_elements.empty()) {
            content << "BT\n"; // Begin text
            content << "/F1 12 Tf\n"; // Set font
            
            for (const auto& text : page.text_elements) {
                // Set text color (non-stroking color for text)
                content << text.color_rgb[0] << " " << text.color_rgb[1] << " " << text.color_rgb[2] << " rg\n";
                // Position text 
                content << "1 0 0 1 " << text.x << " " << text.y << " Tm\n";
                // Show text
                content << "(" << EscapeString(text.text) << ") Tj\n";
            }
            
            content << "ET\n"; // End text
        }
        
        content << "Q\n"; // Restore graphics state
    }
    
    std::string EscapeString(const std::string& str) {
        std::string result;
        for (char c : str) {
            if (c == '(' || c == ')' || c == '\\') {
                result += '\\';
            }
            result += c;
        }
        return result;
    }
    
    void WriteCrossReferenceTable(std::ostringstream& content, const std::vector<size_t>& offsets) {
        content << "xref\n";
        content << "0 " << (offsets.size() + 1) << "\n";
        content << "0000000000 65535 f \n";
        
        for (size_t offset : offsets) {
            content << std::setfill('0') << std::setw(10) << offset << " 00000 n \n";
        }
    }
    
    void WriteTrailer(std::ostringstream& content, size_t xref_offset) {
        content << "trailer\n";
        content << "<<\n";
        content << "/Size " << (objects_.size() + 1) << "\n";
        content << "/Root " << objects_[0].id << " 0 R\n";
        content << ">>\n";
        content << "startxref\n";
        content << xref_offset << "\n";
        content << "%%EOF\n";
    }
};

// PDFGenerator implementation
PDFGenerator::PDFGenerator() : pImpl(std::make_unique<Impl>()) {}

PDFGenerator::~PDFGenerator() = default;

bool PDFGenerator::Initialize(const ProcessingOptions& options, ErrorHandler& error_handler) {
    return pImpl->Initialize(options, error_handler);
}

bool PDFGenerator::CreatePDF(const PostScriptParser& parser, const std::string& output_file, 
                           ErrorHandler& error_handler) {
    return pImpl->CreatePDF(parser, output_file, error_handler);
}

bool PDFGenerator::AddPage(const PDFPageData& page_data) {
    pImpl->pages_.push_back(page_data);
    return true;
}

void PDFGenerator::SetMetadata(const PDFMetadata& metadata) {
    pImpl->metadata_ = metadata;
}

bool PDFGenerator::WritePDF(const std::string& filepath, ErrorHandler& error_handler) {
    return pImpl->WritePDF(filepath, error_handler);
}

size_t PDFGenerator::GetPDFSize() const {
    return 0; // TODO: Implement if needed
}

void PDFGenerator::Clear() {
    pImpl->pages_.clear();
    pImpl->objects_.clear();
    pImpl->next_object_id_ = 1;
}

} // namespace PDFLib 