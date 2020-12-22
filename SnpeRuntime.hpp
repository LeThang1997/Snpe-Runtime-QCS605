
#ifndef  SNPERUNTIME_H
#define SNPERUNTIME_H

#include <vector>
#include <memory>
#include <string>
#include <iostream>


// include snpe header
#include "DlSystem/DlError.hpp"
#include "DlSystem/RuntimeList.hpp"
#include "DlSystem/UserBufferMap.hpp"
#include "DlSystem/UDLFunc.hpp"
#include "DlSystem/IUserBuffer.hpp"
#include "DlContainer/IDlContainer.hpp"
#include "SNPE/SNPE.hpp"
#include "SNPE/SNPEBuilder.hpp"
#include "DiagLog/IDiagLog.hpp"
#include "SNPE/SNPEFactory.hpp"
#include "DlSystem/DlVersion.hpp"
#include "DlSystem/DlEnums.hpp"
#include "DlSystem/String.hpp"
#include "Util.hpp"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "DlSystem/PlatformConfig.hpp"

// include my header
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "ImageProcessing.hpp"
#include "Util.hpp"
#include "PocHCMConfig.hpp"

class SnpeRuntime
{
    private:
        std::unique_ptr<zdl::SNPE::SNPE> snpeRuntine;
    public:
        SnpeRuntime();
        ~SnpeRuntime();
        int initSnpeNetwork(std::string containerPath, std::string modelType, zdl::DlSystem::Runtime_t target_device);
        void excuteSnpeClassifier(std::unique_ptr<zdl::DlSystem::ITensor>& input,
                    std::vector<std::string>& labels, std::vector<ObjectClassifier>& classes);
        void excuteSnpeMobilenetSSD(std::unique_ptr<zdl::DlSystem::ITensor>& input,
                    std::vector<std::string>& labels);
        std::unique_ptr<zdl::DlSystem::ITensor> convertYuv2BgrFloat( char* buf_src, int src_width, int src_height,  int dst_width, int dst_height);
        std::unique_ptr<zdl::DlSystem::ITensor> convertMat2BgrByte(cv::Mat& img);
        std::unique_ptr<zdl::DlSystem::ITensor> convertMat2BgrFloat(cv::Mat& img);  
        std::unique_ptr<zdl::DlSystem::ITensor> loadInputTensorByte(std::string& fileLine);
        std::unique_ptr<zdl::DlSystem::ITensor> loadInputTensorByteToFloat(std::string& fileLine);
        std::unique_ptr<zdl::DlSystem::ITensor> convertYuvtoBgrByte(unsigned char* src_buf, int dst_width, int dst_height);

};
#endif