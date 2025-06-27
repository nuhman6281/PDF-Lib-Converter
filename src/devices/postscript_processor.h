/**
 * @file postscript_processor.h
 * @brief PostScript processor device for handling PostScript and EPS files
 * @version 1.0.0
 * @author PDF_LIB Team
 */

#ifndef POSTSCRIPT_PROCESSOR_H
#define POSTSCRIPT_PROCESSOR_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "../core/device.h"
#include "../core/device_manager.h"

namespace PDFLib {

class PDFEngine;
class ResourceManager;
class PostScriptInterpreter;

/**
 * @brief PostScript processor device for handling PostScript and EPS files
 */
class PostScriptProcessor : public Device {
public:
    /**
     * @brief Constructor
     * @param engine PDF engine instance
     * @param resource_manager Resource manager instance
     */
    PostScriptProcessor(std::shared_ptr<PDFEngine> engine, 
                        std::shared_ptr<ResourceManager> resource_manager);
    
    /**
     * @brief Destructor
     */
    ~PostScriptProcessor() override;
    
    /**
     * @brief Initialize the device
     * @param options Processing options
     * @return true if initialization successful, false otherwise
     */
    bool Initialize(const ProcessingOptions& options) override;
    
    /**
     * @brief Open the processor (Device interface)
     * @return true if opened successfully, false otherwise
     */
    bool Open() override;
    
    /**
     * @brief Open the processor for a PostScript file
     * @param input_file Input PostScript file path
     * @return true if opened successfully, false otherwise
     */
    bool Open(const std::string& input_file);
    
    /**
     * @brief Close the device
     */
    bool Close() override;
    
    /**
     * @brief Process the PostScript file
     * @return true if processing successful, false otherwise
     */
    bool Process();
    
    /**
     * @brief Execute PostScript commands
     * @param commands Vector of PostScript commands
     * @return true if execution successful, false otherwise
     */
    bool ExecuteCommands(const std::vector<std::string>& commands);
    
    /**
     * @brief Get bounding box information
     * @param llx Lower left X coordinate
     * @param lly Lower left Y coordinate
     * @param urx Upper right X coordinate
     * @param ury Upper right Y coordinate
     * @return true if bounding box found, false otherwise
     */
    bool GetBoundingBox(double& llx, double& lly, double& urx, double& ury);
    
    /**
     * @brief Set PostScript variables
     * @param variables Map of variable names to values
     */
    void SetVariables(const std::map<std::string, std::string>& variables);
    
    /**
     * @brief Get device type
     * @return Device type
     */
    DeviceType GetType() const override { return DeviceType::POSTSCRIPT; }
    
    /**
     * @brief Check if device is open
     * @return true if open, false otherwise
     */
    bool IsOpen() const override;
    
    /**
     * @brief Write page data to the device
     * @param page_data Page data to write
     * @param page_number Page number (1-based)
     * @return true if successful, false otherwise
     */
    bool WritePage(const std::vector<uint8_t>& page_data, int page_number) override;
    
    /**
     * @brief Finalize the output
     * @return true if successful, false otherwise
     */
    bool Finalize() override;
    
    /**
     * @brief Get device name
     * @return Device name
     */
    std::string GetName() const override;
    
    /**
     * @brief Check if device is initialized
     * @return true if initialized, false otherwise
     */
    bool IsInitialized() const override;
    
    /**
     * @brief Get supported file extensions
     * @return Vector of supported extensions
     */
    std::vector<std::string> GetSupportedExtensions() const override;
    
    /**
     * @brief Get device capabilities
     * @return Device capabilities as string
     */
    std::string GetCapabilities() const override;
    
    /**
     * @brief Set device-specific parameter
     * @param name Parameter name
     * @param value Parameter value
     * @return true if successful, false otherwise
     */
    bool SetParameter(const std::string& name, const std::string& value) override;
    
    /**
     * @brief Get device-specific parameter
     * @param name Parameter name
     * @return Parameter value, empty string if not found
     */
    std::string GetParameter(const std::string& name) const override;
    
    /**
     * @brief Check if file is EPS format
     * @param filepath File path to check
     * @return true if EPS format, false otherwise
     */
    static bool IsEPSFile(const std::string& filepath);
    
    /**
     * @brief Check if file is PostScript format
     * @param filepath File path to check
     * @return true if PostScript format, false otherwise
     */
    static bool IsPostScriptFile(const std::string& filepath);

private:
    std::shared_ptr<PDFEngine> pdf_engine_;
    std::shared_ptr<ResourceManager> resource_manager_;
    std::shared_ptr<PostScriptInterpreter> interpreter_;
    std::string input_file_;
    bool is_open_;
    bool is_initialized_;
    
    // PostScript variables
    std::map<std::string, std::string> variables_;
    
    // Bounding box information
    double bbox_llx_, bbox_lly_, bbox_urx_, bbox_ury_;
    bool bbox_valid_;
    
    /**
     * @brief Parse PostScript file header
     * @return true if parsing successful, false otherwise
     */
    bool ParseHeader();
    
    /**
     * @brief Extract bounding box from PostScript comments
     * @return true if bounding box found, false otherwise
     */
    bool ExtractBoundingBox();
    
    /**
     * @brief Initialize PostScript interpreter
     * @return true if initialization successful, false otherwise
     */
    bool InitializeInterpreter();
    
    /**
     * @brief Extract bounding box from a PostScript line
     * @param line Line containing bounding box information
     * @return true if extraction successful, false otherwise
     */
    bool ExtractBoundingBoxFromLine(const std::string& line);
};

} // namespace PDFLib

#endif // POSTSCRIPT_PROCESSOR_H 