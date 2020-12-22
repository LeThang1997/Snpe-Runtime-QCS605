
/******************************************************************************** 
 Copyright (C) 2020, LE MANH THANG. All rights reserved.
 Module: SnpeRuntime.cpp
 Author: LE MANH THANG
 Created: 21/12/2020
 Description: 
********************************************************************************/
#ifndef HATCLASSIFIER_H
#define HATCLASSIFIER_H

#include "SnpeRuntime.hpp"

class HatClassifier : SnpeRuntime
{
    private:
        std::vector<std::string> labels;
    public:
        HatClassifier();
        int initHatClassifier();
        int excuteHatClassifier(cv::Mat& img, std::string& className, float& confidence);
};

#endif