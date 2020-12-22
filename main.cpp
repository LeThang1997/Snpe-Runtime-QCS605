/******************************************************************************** 
 Copyright (C) 2020, LE MANH THANG. All rights reserved.
 Module: main.cpp
 Author: LE MANH THANG
 Created: 21/12/2020
 Description: 
********************************************************************************/
#include <fstream>
#include "SnpeRuntime.hpp"
#include "PocHCMConfig.hpp"
#include "HatClassifier.hpp"

int main()
{
    printf("Snpe Engine created by ThangLMb\n");
    

    // Load yuv
    double start, time;
    std::ifstream file_in;
    file_in.open("/data/bkavai/snpe/dataset/image.yuv", std::ios::binary);
    std::filebuf *p_filebuf = file_in.rdbuf();
    size_t size = p_filebuf->pubseekoff(0, std::ios::end, std::ios::in);
    p_filebuf->pubseekpos(0, std::ios::in);
    printf("size: %d\n", size);
    char *buf_src = new char[size];
    p_filebuf->sgetn(buf_src, size);
    
    HatClassifier hat;
    hat.initHatClassifier();
    while(1)
    {
        cv::Mat img = cv::imread("/data/bkavai/snpe/dataset/hat.jpg", cv::IMREAD_COLOR);
        start = static_cast<double>(cv::getTickCount());
        std::string className;
        float confidence; 
        hat.excuteHatClassifier(img, className, confidence);
        std::cout << "The most confident belongs to class " << className << " with score " << confidence << std::endl;
        time = ((double)cv::getTickCount() - start) / cv::getTickFrequency();
        printf("Excuting network spent: %f\n", time);
    }
    return 0;
}
