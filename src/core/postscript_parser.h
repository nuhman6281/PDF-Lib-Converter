/**
 * @file postscript_parser.h
 * @brief PostScript language parser for PDF_LIB
 */

#ifndef PDF_LIB_POSTSCRIPT_PARSER_H
#define PDF_LIB_POSTSCRIPT_PARSER_H

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <memory>

namespace PDFLib {

// Forward declarations
class ErrorHandler;
class PDFGenerator;

/**
 * @brief PostScript graphics state
 */
struct GraphicsState {
    double current_x = 0.0;
    double current_y = 0.0;
    double line_width = 1.0;
    double color_rgb[3] = {0.0, 0.0, 0.0}; // Black by default
    bool gsave_active = false;
    std::string font_name = "Helvetica";
    double font_size = 12.0;
    double rotation = 0.0;
    std::vector<double> transformation_matrix = {1.0, 0.0, 0.0, 1.0, 0.0, 0.0};
};

/**
 * @brief PostScript path element
 */
struct PathElement {
    enum Type { MOVE_TO, LINE_TO, CURVE_TO, CLOSE_PATH };
    Type type;
    std::vector<double> points;
};

/**
 * @brief PostScript text element
 */
struct TextElement {
    std::string text;
    double x, y;
    std::string font_name;
    double font_size;
    std::vector<double> color_rgb;
};

/**
 * @brief PostScript bounding box
 */
struct BoundingBox {
    double x1 = 0.0, y1 = 0.0, x2 = 612.0, y2 = 792.0; // Default A4 size
    bool valid = false;
};

/**
 * @brief PostScript coordinate transform
 */
struct CoordinateTransform {
    double scale_x = 1.0;
    double scale_y = 1.0;
    double offset_x = 0.0;
    double offset_y = 0.0;
    double page_width = 612.0;  // Default A4 width
    double page_height = 792.0; // Default A4 height
};

/**
 * @brief PostScript parser class
 */
class PostScriptParser {
public:
    PostScriptParser();
    ~PostScriptParser();
    
    /**
     * @brief Parse PostScript file
     * @param filepath Path to PostScript file
     * @param error_handler Error handler instance
     * @return true if parsing successful
     */
    bool ParseFile(const std::string& filepath, ErrorHandler& error_handler);
    
    /**
     * @brief Parse PostScript content from string
     * @param content PostScript content
     * @param error_handler Error handler instance
     * @return true if parsing successful
     */
    bool ParseContent(const std::string& content, ErrorHandler& error_handler);
    
    /**
     * @brief Get parsed page count
     * @return Number of pages
     */
    int GetPageCount() const;
    
    /**
     * @brief Get page dimensions
     * @param page_index Page index (0-based)
     * @param width Output width
     * @param height Output height
     * @return true if page exists
     */
    bool GetPageDimensions(int page_index, double& width, double& height) const;
    
    /**
     * @brief Get page paths
     * @param page_index Page index (0-based)
     * @return Vector of path elements
     */
    std::vector<PathElement> GetPagePaths(int page_index) const;
    
    /**
     * @brief Get page text elements
     * @param page_index Page index (0-based)
     * @return Vector of text elements
     */
    std::vector<TextElement> GetPageText(int page_index) const;
    
    /**
     * @brief Get bounding box
     * @param x1 Output x1 coordinate
     * @param y1 Output y1 coordinate
     * @param x2 Output x2 coordinate
     * @param y2 Output y2 coordinate
     * @return true if bounding box found
     */
    bool GetBoundingBox(double& x1, double& y1, double& x2, double& y2) const;
    
    /**
     * @brief Check if file has DSC comments
     * @return true if DSC compliant
     */
    bool IsDSCCompliant() const;
    
    /**
     * @brief Get document title
     * @return Document title or empty string
     */
    std::string GetTitle() const;
    
    /**
     * @brief Get document creator
     * @return Document creator or empty string
     */
    std::string GetCreator() const;
    
    // Coordinate transformation methods
    void TransformCoordinates(double& x, double& y);
    void SetupCoordinateTransform();
    void ParseBoundingBox(const std::string& line);
    
    // Getters
    const std::vector<PathElement>& GetPaths() const { return paths; }
    const std::vector<TextElement>& GetTextElements() const { return text_elements; }
    const BoundingBox& GetBoundingBox() const { return bounding_box; }
    const CoordinateTransform& GetCoordinateTransform() const { return coord_transform; }

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
    std::vector<PathElement> paths;
    std::vector<TextElement> text_elements;
    GraphicsState graphics_state;
    BoundingBox bounding_box;
    CoordinateTransform coord_transform;
};

} // namespace PDFLib

#endif // PDF_LIB_POSTSCRIPT_PARSER_H 