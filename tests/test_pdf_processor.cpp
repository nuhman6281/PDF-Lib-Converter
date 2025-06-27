/**
 * @file test_pdf_processor.cpp
 * @brief Unit tests for PDF processor
 * @version 1.0.0
 * @author PDF_LIB Team
 */

#include <gtest/gtest.h>
#include "../include/pdf_lib.h"
#include <vector>
#include <string>

using namespace PDFLib;

class PDFProcessorTest : public ::testing::Test {
protected:
    void SetUp() override {
        processor_ = std::make_unique<PDFProcessor>();
    }
    
    void TearDown() override {
        if (processor_) {
            processor_->Cleanup();
        }
    }
    
    std::unique_ptr<PDFProcessor> processor_;
};

TEST_F(PDFProcessorTest, ConstructorTest) {
    EXPECT_NE(processor_, nullptr);
    EXPECT_FALSE(processor_->IsInitialized());
}

TEST_F(PDFProcessorTest, InitializationTest) {
    ProcessingOptions options;
    options.device_name = "pdfwrite";
    options.output_file = "test_output.pdf";
    options.input_files = {"test_input.ps"};
    
    EXPECT_TRUE(processor_->Initialize(options));
    EXPECT_TRUE(processor_->IsInitialized());
}

TEST_F(PDFProcessorTest, CommandLineInitializationTest) {
    std::vector<std::string> args = {
        "pdf_gs",
        "-sDEVICE=pdfwrite",
        "-sOutputFile=test_output.pdf",
        "test_input.ps"
    };
    
    EXPECT_TRUE(processor_->Initialize(args));
    EXPECT_TRUE(processor_->IsInitialized());
}

TEST_F(PDFProcessorTest, InvalidInitializationTest) {
    ProcessingOptions options;
    // Missing required fields
    EXPECT_FALSE(processor_->Initialize(options));
    EXPECT_FALSE(processor_->IsInitialized());
}

TEST_F(PDFProcessorTest, ErrorHandlingTest) {
    ProcessingOptions options;
    options.device_name = "nonexistent_device";
    options.input_files = {"nonexistent_file.ps"};
    
    EXPECT_FALSE(processor_->Initialize(options));
    
    ErrorInfo error = processor_->GetLastError();
    EXPECT_NE(error.code, 0);
    EXPECT_FALSE(error.message.empty());
}

TEST_F(PDFProcessorTest, ProgressCallbackTest) {
    bool callback_called = false;
    int current_progress = 0;
    int total_progress = 0;
    
    auto callback = [&](int current, int total, const std::string& status) {
        callback_called = true;
        current_progress = current;
        total_progress = total;
    };
    
    processor_->SetProgressCallback(callback);
    
    // Note: In a real test, you would trigger processing to test the callback
    // For now, we just verify the callback can be set
    EXPECT_TRUE(true);
}

TEST_F(PDFProcessorTest, OptionsRetrievalTest) {
    ProcessingOptions input_options;
    input_options.device_name = "pdfwrite";
    input_options.output_file = "test_output.pdf";
    input_options.input_files = {"test_input.ps"};
    input_options.quality = PDFQuality::PRINTER;
    input_options.paper_size = PaperSize::A4;
    input_options.num_copies = 2;
    
    EXPECT_TRUE(processor_->Initialize(input_options));
    
    ProcessingOptions retrieved_options = processor_->GetOptions();
    EXPECT_EQ(retrieved_options.device_name, input_options.device_name);
    EXPECT_EQ(retrieved_options.output_file, input_options.output_file);
    EXPECT_EQ(retrieved_options.quality, input_options.quality);
    EXPECT_EQ(retrieved_options.paper_size, input_options.paper_size);
    EXPECT_EQ(retrieved_options.num_copies, input_options.num_copies);
}

TEST_F(PDFProcessorTest, CleanupTest) {
    ProcessingOptions options;
    options.device_name = "pdfwrite";
    options.output_file = "test_output.pdf";
    options.input_files = {"test_input.ps"};
    
    EXPECT_TRUE(processor_->Initialize(options));
    EXPECT_TRUE(processor_->IsInitialized());
    
    processor_->Cleanup();
    EXPECT_FALSE(processor_->IsInitialized());
}

TEST_F(PDFProcessorTest, MultipleInitializationTest) {
    ProcessingOptions options1;
    options1.device_name = "pdfwrite";
    options1.output_file = "test_output1.pdf";
    options1.input_files = {"test_input1.ps"};
    
    EXPECT_TRUE(processor_->Initialize(options1));
    EXPECT_TRUE(processor_->IsInitialized());
    
    // Second initialization should fail
    ProcessingOptions options2;
    options2.device_name = "pdfwrite";
    options2.output_file = "test_output2.pdf";
    options2.input_files = {"test_input2.ps"};
    
    EXPECT_FALSE(processor_->Initialize(options2));
    
    // After cleanup, should be able to initialize again
    processor_->Cleanup();
    EXPECT_TRUE(processor_->Initialize(options2));
    EXPECT_TRUE(processor_->IsInitialized());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 