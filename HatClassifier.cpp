
/******************************************************************************** 
 Copyright (C) 2020, LE MANH THANG. All rights reserved.
 Module: SnpeRuntime.cpp
 Author: LE MANH THANG
 Created: 21/12/2020
 Description: 
********************************************************************************/
#include "HatClassifier.hpp"
#include "PocHCMConfig.hpp"
#include <fstream>

int loadLabel(std::string nameFile, std::vector<std::string>& labels)
{
	std::ifstream fileLabel(nameFile);
	if(!fileLabel)
    {
        printf("Reading file label error \n");
        return -1;
    }
    while(fileLabel.is_open())
    {
        std::string line;
        while (std::getline(fileLabel, line)) 
        {
            labels.push_back(line);
        }
        fileLabel.close();
    }
	return 0;
}
HatClassifier::HatClassifier():SnpeRuntime()
{
    
}
int HatClassifier::initHatClassifier()
{
    zdl::DlSystem::Runtime_t target_device =  zdl::DlSystem::Runtime_t::GPU;
    if(initSnpeNetwork( PATH_MODEL_COLORFASHION, "classifier", target_device) != 0)
    {
        printf("Initiate Hat classifier error\n");
        return -1;
    }
    loadLabel(PATH_LABEL_COLORFASHION, labels);
    printf("There are %d in label file\n", labels.size());
    printf("Initiated Snpe runtime successfully\n");
    return 0;
}
int HatClassifier::excuteHatClassifier(cv::Mat& img, std::string& className, float& confidence)
{
    
    //std::unique_ptr<zdl::DlSystem::ITensor> inputTensor = convertMat2BgrByte(img);
    std::string inputraw = "/data/bkavai/snpe/dataset/red.raw";
    std::unique_ptr<zdl::DlSystem::ITensor> inputTensor = loadInputTensorByteToFloat(inputraw);
    std::vector<ObjectClassifier> classes;
    excuteSnpeClassifier( inputTensor, this->labels, classes);
    float maxScore = 0.0;
    for(int i = 0; i < classes.size(); i++)
    {
        if(maxScore < classes[i].score)
        {
            maxScore = classes[i].score;
            confidence = classes[i].score;
            className  = classes[i].label;
        }
    }
    return 0;
}