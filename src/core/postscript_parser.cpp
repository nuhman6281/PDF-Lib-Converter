/**
 * @file postscript_parser.cpp
 * @brief PostScript language parser implementation
 */

#include "postscript_parser.h"
#include "error_handler.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace PDFLib {

class PostScriptParser::Impl {
public:
    struct Page {
        double width = 595.276;  // A4 default
        double height = 841.890;
        std::vector<PathElement> paths;
        std::vector<TextElement> text_elements;
    };
    
    std::vector<Page> pages_;
    std::string title_;
    std::string creator_;
    double bbox_x1_ = 0, bbox_y1_ = 0, bbox_x2_ = 595.276, bbox_y2_ = 841.890;
    bool dsc_compliant_ = false;
    
    // Coordinate transformation parameters
    double transform_scale_ = 1.0;
    double transform_offset_x_ = 0.0;
    double transform_offset_y_ = 0.0;
    double transform_pdf_height_ = 842.0;
    
    GraphicsState current_state_;
    std::stack<GraphicsState> state_stack_;
    std::vector<PathElement> current_path_;
    
    bool ParseFile(const std::string& filepath, ErrorHandler& error_handler) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            error_handler.SetError(-1, "Cannot open PostScript file: " + filepath);
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        std::cout << "DEBUG: Starting to parse PostScript file: " << filepath << std::endl;
        std::cout << "DEBUG: File size: " << content.length() << " characters" << std::endl;
        
        bool result = ParseContent(content, error_handler);
        
        std::cout << "DEBUG: Parsing complete. Found " << pages_.size() << " pages" << std::endl;
        for (size_t i = 0; i < pages_.size(); ++i) {
            std::cout << "DEBUG: Page " << i << " has " << pages_[i].paths.size() << " paths and " 
                      << pages_[i].text_elements.size() << " text elements" << std::endl;
        }
        
        return result;
    }
    
    bool ParseContent(const std::string& content, ErrorHandler& error_handler) {
        try {
            // Clear previous data
            pages_.clear();
            title_.clear();
            creator_.clear();
            dsc_compliant_ = false;
            
            // Split content into lines
            std::vector<std::string> lines = SplitLines(content);
            
            // Parse DSC comments first
            ParseDSCComments(lines);
            
            // Setup coordinate transformation based on bounding box
            SetupCoordinateTransform();
            
            // Create initial page
            pages_.emplace_back();
            
            // Parse PostScript commands
            for (const auto& line : lines) {
                if (!ParseLine(line, error_handler)) {
                    // Continue parsing even if some lines fail
                    error_handler.Log("Warning: Failed to parse line: " + line, ErrorSeverity::WARNING);
                }
            }
            
            // Ensure we have at least one page
            if (pages_.empty()) {
                pages_.emplace_back();
            }
            
            error_handler.Log("PostScript parsing completed successfully", ErrorSeverity::INFO);
            return true;
            
        } catch (const std::exception& e) {
            error_handler.SetError(-2, "PostScript parsing error: " + std::string(e.what()));
            return false;
        }
    }
    
private:
    std::vector<std::string> SplitLines(const std::string& content) {
        std::vector<std::string> lines;
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            // Remove carriage return if present
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            lines.push_back(line);
        }
        
        return lines;
    }
    
    void ParseDSCComments(const std::vector<std::string>& lines) {
        for (const auto& line : lines) {
            if (line.substr(0, 2) == "%%") {
                dsc_compliant_ = true;
                
                if (line.substr(0, 8) == "%%Title:") {
                    title_ = Trim(line.substr(8));
                } else if (line.substr(0, 10) == "%%Creator:") {
                    creator_ = Trim(line.substr(10));
                } else if (line.substr(0, 14) == "%%BoundingBox:") {
                    ParseBoundingBox(line);
                }
            }
        }
    }
    
    void ParseBoundingBox(const std::string& line) {
        std::regex bbox_regex(R"(%%BoundingBox:\s*(\d+)\s+(\d+)\s+(\d+)\s+(\d+))");
        std::smatch match;
        
        if (std::regex_search(line, match, bbox_regex)) {
            bbox_x1_ = std::stod(match[1].str());
            bbox_y1_ = std::stod(match[2].str());
            bbox_x2_ = std::stod(match[3].str());
            bbox_y2_ = std::stod(match[4].str());
            
            std::cout << "Parsed bounding box: " << bbox_x1_ << " " << bbox_y1_ 
                      << " " << bbox_x2_ << " " << bbox_y2_ << std::endl;
            
            // Update page dimensions if we have pages
            if (!pages_.empty()) {
                pages_[0].width = bbox_x2_ - bbox_x1_;
                pages_[0].height = bbox_y2_ - bbox_y1_;
            }
        }
    }
    
    bool ParseLine(const std::string& line, ErrorHandler& error_handler) {
        std::string trimmed = Trim(line);
        
        // Skip empty lines and comments
        if (trimmed.empty() || trimmed[0] == '%') {
            return true;
        }
        
        // Tokenize the line
        std::vector<std::string> tokens = Tokenize(trimmed);
        if (tokens.empty()) {
            return true;
        }
        
        // Parse commands
        for (size_t i = 0; i < tokens.size(); ++i) {
            const std::string& token = tokens[i];
            
            try {
                // Graphics state commands (full and abbreviated)
                if (token == "gsave" || token == "q") {
                    state_stack_.push(current_state_);
                } else if (token == "grestore" || token == "Q") {
                    if (!state_stack_.empty()) {
                        current_state_ = state_stack_.top();
                        state_stack_.pop();
                    }
                } else if ((token == "setlinewidth" || token == "w") && i > 0 && IsNumeric(tokens[i-1])) {
                    current_state_.line_width = std::stod(tokens[i-1]);
                } else if ((token == "setrgbcolor" || token == "rg") && i >= 3 && 
                          IsNumeric(tokens[i-3]) && IsNumeric(tokens[i-2]) && IsNumeric(tokens[i-1])) {
                    current_state_.color_rgb[0] = std::stod(tokens[i-3]);
                    current_state_.color_rgb[1] = std::stod(tokens[i-2]);
                    current_state_.color_rgb[2] = std::stod(tokens[i-1]);
                }
                // Path construction commands (full and abbreviated)
                else if ((token == "moveto" || token == "m") && i >= 2 && 
                        IsNumeric(tokens[i-2]) && IsNumeric(tokens[i-1])) {
                    double x = std::stod(tokens[i-2]);
                    double y = std::stod(tokens[i-1]);
                    
                    std::cout << "Processing moveto: (" << x << "," << y << ")" << std::endl;
                    
                    // Apply coordinate transformation
                    TransformCoordinates(x, y);
                    
                    current_state_.current_x = x;
                    current_state_.current_y = y;
                    
                    PathElement element;
                    element.type = PathElement::MOVE_TO;
                    element.points = {x, y};
                    current_path_.push_back(element);
                    
                    std::cout << "Added moveto path element" << std::endl;
                    
                } else if ((token == "lineto" || token == "l") && i >= 2 && 
                          IsNumeric(tokens[i-2]) && IsNumeric(tokens[i-1])) {
                    double x = std::stod(tokens[i-2]);
                    double y = std::stod(tokens[i-1]);
                    
                    // Apply coordinate transformation
                    TransformCoordinates(x, y);
                    
                    current_state_.current_x = x;
                    current_state_.current_y = y;
                    
                    PathElement element;
                    element.type = PathElement::LINE_TO;
                    element.points = {x, y};
                    current_path_.push_back(element);
                    
                } else if ((token == "curveto" || token == "c") && i >= 6 && 
                          IsNumeric(tokens[i-6]) && IsNumeric(tokens[i-5]) && 
                          IsNumeric(tokens[i-4]) && IsNumeric(tokens[i-3]) && 
                          IsNumeric(tokens[i-2]) && IsNumeric(tokens[i-1])) {
                    double x1 = std::stod(tokens[i-6]);
                    double y1 = std::stod(tokens[i-5]);
                    double x2 = std::stod(tokens[i-4]);
                    double y2 = std::stod(tokens[i-3]);
                    double x3 = std::stod(tokens[i-2]);
                    double y3 = std::stod(tokens[i-1]);
                    
                    // Apply coordinate transformation to all control points
                    TransformCoordinates(x1, y1);
                    TransformCoordinates(x2, y2);
                    TransformCoordinates(x3, y3);
                    
                    PathElement element;
                    element.type = PathElement::CURVE_TO;
                    element.points = {x1, y1, x2, y2, x3, y3};
                    current_path_.push_back(element);
                    current_state_.current_x = x3;
                    current_state_.current_y = y3;
                    
                } else if (token == "closepath" || token == "h") {
                    PathElement element;
                    element.type = PathElement::CLOSE_PATH;
                    current_path_.push_back(element);
                    
                } else if (token == "stroke" || token == "s") {
                    // Complete current path and add to page
                    if (!current_path_.empty() && !pages_.empty()) {
                        for (const auto& path_elem : current_path_) {
                            pages_.back().paths.push_back(path_elem);
                        }
                        current_path_.clear();
                    }
                    
                } else if (token == "fill" || token == "f" || token == "F") {
                    // Commit current path to page
                    if (!pages_.empty() && !current_path_.empty()) {
                        for (const auto& element : current_path_) {
                            pages_.back().paths.push_back(element);
                        }
                    }
                    current_path_.clear();
                    
                }
                // Text commands
                else if ((token == "show" || token == "Tj") && i > 0) {
                    // Extract text from previous token (should be a string in parentheses)
                    std::string text_token = tokens[i-1];
                    if (text_token.front() == '(' && text_token.back() == ')') {
                        std::string text = text_token.substr(1, text_token.length() - 2);
                        
                        double x = current_state_.current_x;
                        double y = current_state_.current_y;
                        
                        // Apply coordinate transformation to text position
                        TransformCoordinates(x, y);
                        
                        TextElement text_element;
                        text_element.text = text;
                        text_element.x = x;
                        text_element.y = y;
                        text_element.font_name = current_state_.font_name;
                        text_element.font_size = current_state_.font_size;
                        text_element.color_rgb[0] = current_state_.color_rgb[0];
                        text_element.color_rgb[1] = current_state_.color_rgb[1];
                        text_element.color_rgb[2] = current_state_.color_rgb[2];
                        
                        if (!pages_.empty()) {
                            pages_.back().text_elements.push_back(text_element);
                        }
                    }
                }
                // Page commands
                else if (token == "showpage") {
                    // Start new page
                    pages_.emplace_back();
                }
                
            } catch (const std::exception& e) {
                error_handler.Log("Error parsing token '" + token + "': " + e.what(), 
                                ErrorSeverity::WARNING);
            }
        }
        
        return true;
    }
    
    std::vector<std::string> Tokenize(const std::string& line) {
        std::vector<std::string> tokens;
        std::istringstream iss(line);
        std::string token;
        
        while (iss >> token) {
            // Handle strings in parentheses as single tokens
            if (token.front() == '(' && token.back() != ')') {
                std::string rest;
                while (iss >> rest) {
                    token += " " + rest;
                    if (rest.back() == ')') {
                        break;
                    }
                }
            }
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    std::string Trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            return "";
        }
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    bool IsNumeric(const std::string& str) {
        if (str.empty()) return false;
        
        try {
            std::stod(str);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    void SetupCoordinateTransform() {
        // Calculate PostScript dimensions
        double ps_width = bbox_x2_ - bbox_x1_;
        double ps_height = bbox_y2_ - bbox_y1_;
        
        // Set PDF page size (A4: 595 x 842 points)
        double pdf_width = 595.0;
        double pdf_height = 842.0;
        
        // Calculate scale factors to fit content on page
        double scale_x = pdf_width / ps_width;
        double scale_y = pdf_height / ps_height;
        
        // Use the smaller scale to maintain aspect ratio
        double scale = std::min(scale_x, scale_y);
        
        // Calculate centering offsets
        double scaled_width = ps_width * scale;
        double scaled_height = ps_height * scale;
        double offset_x = (pdf_width - scaled_width) / 2.0;
        double offset_y = (pdf_height - scaled_height) / 2.0;
        
        std::cout << "Coordinate transform: scale=" << scale << " offset=(" << offset_x 
                  << "," << offset_y << ")" << std::endl;
        
        // Store transform parameters for use in path processing
        transform_scale_ = scale;
        transform_offset_x_ = offset_x;
        transform_offset_y_ = offset_y;
        transform_pdf_height_ = pdf_height;
    }
    
    void TransformCoordinates(double& x, double& y) {
        // Transform from PostScript coordinates to PDF coordinates
        double original_x = x;
        double original_y = y;
        
        x = x * transform_scale_ + transform_offset_x_;
        y = transform_pdf_height_ - (y * transform_scale_ + transform_offset_y_);
        
        std::cout << "Transform: (" << original_x << "," << original_y << ") -> (" << x << "," << y << ")" << std::endl;
    }
};

// PostScriptParser implementation
PostScriptParser::PostScriptParser() : pImpl(std::make_unique<Impl>()) {}

PostScriptParser::~PostScriptParser() = default;

bool PostScriptParser::ParseFile(const std::string& filepath, ErrorHandler& error_handler) {
    return pImpl->ParseFile(filepath, error_handler);
}

bool PostScriptParser::ParseContent(const std::string& content, ErrorHandler& error_handler) {
    return pImpl->ParseContent(content, error_handler);
}

int PostScriptParser::GetPageCount() const {
    return static_cast<int>(pImpl->pages_.size());
}

bool PostScriptParser::GetPageDimensions(int page_index, double& width, double& height) const {
    if (page_index < 0 || page_index >= static_cast<int>(pImpl->pages_.size())) {
        return false;
    }
    
    width = pImpl->pages_[page_index].width;
    height = pImpl->pages_[page_index].height;
    return true;
}

std::vector<PathElement> PostScriptParser::GetPagePaths(int page_index) const {
    if (page_index < 0 || page_index >= static_cast<int>(pImpl->pages_.size())) {
        return {};
    }
    
    return pImpl->pages_[page_index].paths;
}

std::vector<TextElement> PostScriptParser::GetPageText(int page_index) const {
    if (page_index < 0 || page_index >= static_cast<int>(pImpl->pages_.size())) {
        return {};
    }
    
    return pImpl->pages_[page_index].text_elements;
}

bool PostScriptParser::GetBoundingBox(double& x1, double& y1, double& x2, double& y2) const {
    x1 = pImpl->bbox_x1_;
    y1 = pImpl->bbox_y1_;
    x2 = pImpl->bbox_x2_;
    y2 = pImpl->bbox_y2_;
    return true;
}

bool PostScriptParser::IsDSCCompliant() const {
    return pImpl->dsc_compliant_;
}

std::string PostScriptParser::GetTitle() const {
    return pImpl->title_;
}

std::string PostScriptParser::GetCreator() const {
    return pImpl->creator_;
}

} // namespace PDFLib 