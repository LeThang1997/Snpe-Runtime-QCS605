/******************************************************************************** 
 Copyright (C) 2020, LE MANH THANG. All rights reserved.
 Module: SnpeRuntime.cpp
 Author: LE MANH THANG
 Created: 21/12/2020
 Description: 
********************************************************************************/
#include "SnpeRuntime.hpp"

SnpeRuntime::SnpeRuntime() : snpeRuntine()
{
    this->snpeRuntine = std::unique_ptr<zdl::SNPE::SNPE>();
}
SnpeRuntime::~SnpeRuntime()
{

}
zdl::DlSystem::Runtime_t checkRuntime(zdl::DlSystem::Runtime_t runtime)
{
    static zdl::DlSystem::Version_t Version = zdl::SNPE::SNPEFactory::getLibraryVersion();

    std::cout << "SNPE Version: " << Version.asString().c_str() << std::endl; //Print Version number

    if (!zdl::SNPE::SNPEFactory::isRuntimeAvailable(runtime))
    {
        std::cerr << "Selected runtime not present. Falling back to CPU." << std::endl;
        runtime = zdl::DlSystem::Runtime_t::CPU;
    }

    return runtime;
}

std::unique_ptr<zdl::DlContainer::IDlContainer> loadContainerFromFile(std::string containerPath)
{
    std::unique_ptr<zdl::DlContainer::IDlContainer> container;
    container = zdl::DlContainer::IDlContainer::open(containerPath);
    return container;
}

std::unique_ptr<zdl::SNPE::SNPE> setBuilderOptions(std::unique_ptr<zdl::DlContainer::IDlContainer>& container,
                                                   zdl::DlSystem::RuntimeList runtimeList,
                                                   zdl::DlSystem::StringList outputs)
{
    std::unique_ptr<zdl::SNPE::SNPE> snpe;
    zdl::SNPE::SNPEBuilder snpeBuilder(container.get());
    snpe = snpeBuilder.setOutputLayers(outputs)
            .setCPUFallbackMode(true)
            .setRuntimeProcessorOrder(runtimeList)
            .build();
    return snpe;
}

std::unique_ptr<zdl::DlSystem::ITensor> SnpeRuntime::loadInputTensorByte(std::string& fileLine)
{
    std::unique_ptr<zdl::DlSystem::ITensor> input;
    const auto &strList_opt = this->snpeRuntine->getInputTensorNames();
    if (!strList_opt) throw std::runtime_error("Error obtaining Input tensor names");
    const auto &strList = *strList_opt;
    // Make sure the network requires only a single input
    assert (strList.size() == 1);

    // If the network has a single input, each line represents the input file to be loaded for that input
    //std::vector<float> inputVec;
    std::vector< unsigned char> inputVec;
    
    std::cout << "Processing DNN Input: " << fileLine << "\n";
    //std::vector<float> loadedFile = loadFloatDataFile(filePath);
    std::vector<unsigned char> loadedFile = loadByteDataFile(fileLine);
    inputVec.insert(inputVec.end(), loadedFile.begin(), loadedFile.end());
    
    /* Create an input tensor that is correctly sized to hold the input of the network. Dimensions that have no fixed size will be represented with a value of 0. */
    const auto &inputDims_opt = this->snpeRuntine->getInputDimensions(strList.at(0));
    const auto &inputShape = *inputDims_opt;

    /* Calculate the total number of elements that can be stored in the tensor so that we can check that the input contains the expected number of elements.
       With the input dimensions computed create a tensor to convey the input into the network. */
    input = zdl::SNPE::SNPEFactory::getTensorFactory().createTensor(inputShape);
    //Padding the input vector so as to make the size of the vector to equal to an integer multiple of the batch size

    if (input->getSize() != inputVec.size()) {
        std::cerr << "Size of input does not match network.\n"
                  << "Expecting: " << input->getSize() << "\n"
                  << "Got: " << inputVec.size() << "\n";
        return nullptr;
    }
    /* Copy the oaded input file contents into the networks input tensor. SNPE's ITensor supports C++ STL functions like std::copy() */
    
    std::copy(inputVec.begin(), inputVec.end(), input->begin());
    return input;
}

std::unique_ptr<zdl::DlSystem::ITensor> SnpeRuntime::loadInputTensorByteToFloat(std::string& fileLine)
{
    std::unique_ptr<zdl::DlSystem::ITensor> input;
    const auto &strList_opt = this->snpeRuntine->getInputTensorNames();
    if (!strList_opt) throw std::runtime_error("Error obtaining Input tensor names");
    const auto &strList = *strList_opt;
    // Make sure the network requires only a single input
    assert (strList.size() == 1);

    // If the network has a single input, each line represents the input file to be loaded for that input
    //std::vector<float> inputVec;
    std::vector< unsigned char> inputVec;
    
    std::cout << "Processing DNN Input: " << fileLine << "\n";
    //std::vector<float> loadedFile = loadFloatDataFile(filePath);
    std::vector<unsigned char> loadedFile = loadByteDataFile(fileLine);
    inputVec.insert(inputVec.end(), loadedFile.begin(), loadedFile.end());
    
    /* Create an input tensor that is correctly sized to hold the input of the network. Dimensions that have no fixed size will be represented with a value of 0. */
    const auto &inputDims_opt = this->snpeRuntine->getInputDimensions(strList.at(0));
    const auto &inputShape = *inputDims_opt;

    /* Calculate the total number of elements that can be stored in the tensor so that we can check that the input contains the expected number of elements.
       With the input dimensions computed create a tensor to convey the input into the network. */
    input = zdl::SNPE::SNPEFactory::getTensorFactory().createTensor(inputShape);
    //Padding the input vector so as to make the size of the vector to equal to an integer multiple of the batch size

    if (input->getSize() != inputVec.size()) {
        std::cerr << "Size of input does not match network.\n"
                  << "Expecting: " << input->getSize() << "\n"
                  << "Got: " << inputVec.size() << "\n";
        return nullptr;
    }
    std::vector< float> inputVecFloat;
    for( int i = 0; i < inputVec.size(); i++)
    {        
        float fData = (float)((inputVec[i] - 128.0f) / 128.0f);
        //float fData = (float)((inputVec[i] ) / 255.0f);
        inputVecFloat.push_back(fData);
    }
    /* Copy the oaded input file contents into the networks input tensor. SNPE's ITensor supports C++ STL functions like std::copy() */
    std::copy(inputVecFloat.begin(), inputVecFloat.end(), input->begin());
    return input;
}


int  SnpeRuntime::initSnpeNetwork(std::string containerPath, std::string modelType, zdl::DlSystem::Runtime_t target_device)
{
    //std::unique_ptr<zdl::SNPE::SNPE> snpe;
    std::unique_ptr<zdl::DlContainer::IDlContainer> container = loadContainerFromFile(containerPath);
    if (container == nullptr)
    {
       std::cerr << "Error while opening the container file." << std::endl;
       return -1;
    }
    printf("Loaded dlc file successfully\n");
    // add runtime target
    static zdl::DlSystem::RuntimeList runtimeList;
    static zdl::DlSystem::Runtime_t runtime = checkRuntime(target_device);
    if(runtimeList.empty()) {
        runtimeList.add(runtime);
    }    
    // adding of second layer gives us three more buffers which will have boxes and scores
    zdl::DlSystem::StringList outputs;
    if(modelType == "mobiletnetssd")
    {
        outputs.append("add");
        outputs.append("Postprocessor/BatchMultiClassNonMaxSuppression");
        printf("Mode type is Mobilenet SSD\n");
    }
    else if("classifier")
    {
        printf("Mode type is Classifier\n");
    }
    
    snpeRuntine = setBuilderOptions(container, runtimeList, outputs);
    if (snpeRuntine == nullptr)
    {
       std::cerr << "Error while building SNPE object." << std::endl;
       //return nullptr;
       return -1;
    }
    printf("Configured network successfully\n");
    //return snpe;
    return 0;
}

std::unique_ptr<zdl::DlSystem::ITensor> SnpeRuntime::convertYuv2BgrFloat( char* buf_src, 
                                                                    int src_width, int src_height, int dst_width, int dst_height)
{
    std::unique_ptr<zdl::DlSystem::ITensor> input;
    const auto &strList_opt = this->snpeRuntine->getInputTensorNames();
    if (!strList_opt) throw std::runtime_error("Error obtaining Input tensor names");
    const auto &strList = *strList_opt;
    // Make sure the network requires only a single input
    assert (strList.size() == 1);
    /* Create an input tensor that is correctly sized to hold the input of the network. Dimensions that have no fixed size will be represented with a value of 0. */
    const auto &inputDims_opt = this->snpeRuntine->getInputDimensions(strList.at(0));
    const auto &inputShape = *inputDims_opt;
    /* Calculate the total number of elements that can be stored in the tensor so that we can check that the input contains the expected number of elements.
       With the input dimensions computed create a tensor to convey the input into the network. */
    input = zdl::SNPE::SNPEFactory::getTensorFactory().createTensor(inputShape);
    //Padding the input vector so as to make the size of the vector to equal to an integer multiple of the batch size
    printf("Input network target: %d\n", input->getSize());

    cv::Mat nv_21_mat = cv::Mat(src_height * 1.5, src_width, CV_8UC1, buf_src);
    cv::Mat bgr_mat;
    cv::cvtColor(nv_21_mat, bgr_mat, cv::COLOR_YUV2BGR_NV21);
    cv::Mat bgr_resize;
    cv::resize(bgr_mat, bgr_resize, cv::Size(dst_width, dst_width));
    cv::Mat flat = bgr_resize.reshape(1, bgr_resize.total()*bgr_resize.channels());
    std::vector<char> inputVec = bgr_resize.isContinuous()? flat : flat.clone();

    if (input->getSize() != inputVec.size()) {
        std::cerr << "Size of input does not match network.\n"
                  << "Expecting: " << input->getSize() << "\n"
                  << "Got: " << inputVec.size() << "\n";
        return nullptr;
    }
    std::vector< float> inputVecFloat;
    for( int i = 0; i < inputVec.size(); i++)
    {        
        //float fData = (float)((inputVec[i] - 128.0f) / 128.0f);
        float fData = (float)((inputVec[i] ) / 255.0f);
        inputVecFloat.push_back(fData);
    }
    /* Copy the oaded input file contents into the networks input tensor. SNPE's ITensor supports C++ STL functions like std::copy() */
    std::copy(inputVecFloat.begin(), inputVecFloat.end(), input->begin());
    return input;
}

std::unique_ptr<zdl::DlSystem::ITensor> SnpeRuntime::convertMat2BgrByte(cv::Mat& img)
{
    std::unique_ptr<zdl::DlSystem::ITensor> input;
    const auto &strList_opt = this->snpeRuntine->getInputTensorNames();
    if (!strList_opt) throw std::runtime_error("Error obtaining Input tensor names");
    const auto &strList = *strList_opt;
    // Make sure the network requires only a single input
    assert (strList.size() == 1);
    /* Create an input tensor that is correctly sized to hold the input of the network. Dimensions that have no fixed size will be represented with a value of 0. */
    const auto &inputDims_opt = this->snpeRuntine->getInputDimensions(strList.at(0));
    const auto &inputShape = *inputDims_opt;
    /* Calculate the total number of elements that can be stored in the tensor so that we can check that the input contains the expected number of elements.
       With the input dimensions computed create a tensor to convey the input into the network. */
    input = zdl::SNPE::SNPEFactory::getTensorFactory().createTensor(inputShape);
    //Padding the input vector so as to make the size of the vector to equal to an integer multiple of the batch size
    printf("Input network target: %d with batch %d, w %d, h %d, c %d\n", input->getSize(),inputShape[0], inputShape[1], inputShape[2], inputShape[3] );
    cv::Mat bgr_resize;
    cv::resize(img, bgr_resize, cv::Size((int)inputShape[1], (int)inputShape[2]));
    cv::Mat flat = bgr_resize.reshape(1, bgr_resize.total()*bgr_resize.channels());
    std::vector<char> inputVec = bgr_resize.isContinuous()? flat : flat.clone();

    if (input->getSize() != inputVec.size()) {
        std::cerr << "Size of input does not match network.\n"
                  << "Expecting: " << input->getSize() << "\n"
                  << "Got: " << inputVec.size() << "\n";
        return nullptr;
    }
    /* Copy the oaded input file contents into the networks input tensor. SNPE's ITensor supports C++ STL functions like std::copy() */
    std::copy(inputVec.begin(), inputVec.end(), input->begin());
    return input;
}
std::unique_ptr<zdl::DlSystem::ITensor> SnpeRuntime::convertMat2BgrFloat(cv::Mat& img)
{
    std::unique_ptr<zdl::DlSystem::ITensor> input;
    const auto &strList_opt = this->snpeRuntine->getInputTensorNames();
    if (!strList_opt) throw std::runtime_error("Error obtaining Input tensor names");
    const auto &strList = *strList_opt;
    // Make sure the network requires only a single input
    assert (strList.size() == 1);
    /* Create an input tensor that is correctly sized to hold the input of the network. Dimensions that have no fixed size will be represented with a value of 0. */
    const auto &inputDims_opt = this->snpeRuntine->getInputDimensions(strList.at(0));
    const auto &inputShape = *inputDims_opt;
    /* Calculate the total number of elements that can be stored in the tensor so that we can check that the input contains the expected number of elements.
       With the input dimensions computed create a tensor to convey the input into the network. */
    input = zdl::SNPE::SNPEFactory::getTensorFactory().createTensor(inputShape);
    //std::cout << "Input shape: " << inputShape[0] << inputShape[1] << inputShape[2] << inputShape[3] << std::endl;
    //Padding the input vector so as to make the size of the vector to equal to an integer multiple of the batch size
    
    printf("Input network target: %d with batch %d, w %d, h %d, c %d\n", input->getSize(),inputShape[0], inputShape[1], inputShape[2], inputShape[3] );
    cv::Mat bgr_resize;
    cv::resize(img, bgr_resize, cv::Size((int)inputShape[1], (int)inputShape[2]));
    cv::Mat flat = bgr_resize.reshape(1, bgr_resize.total()*bgr_resize.channels());
    std::vector<char> inputVec = bgr_resize.isContinuous()? flat : flat.clone();

    if (input->getSize() != inputVec.size()) {
        std::cerr << "Size of input does not match network.\n"
                  << "Expecting: " << input->getSize() << "\n"
                  << "Got: " << inputVec.size() << "\n";
        return nullptr;
    }
    std::vector< float> inputVecFloat;
    for( int i = 0; i < inputVec.size(); i++)
    {        
        //float fData = (float)((inputVec[i] - 128.0f) / 128.0f);
        float fData = (float)((inputVec[i] ) / 255.0f);
        inputVecFloat.push_back(fData);
    }
    /* Copy the oaded input file contents into the networks input tensor. SNPE's ITensor supports C++ STL functions like std::copy() */
    std::copy(inputVecFloat.begin(), inputVecFloat.end(), input->begin());
    return input;
}

void SnpeRuntime::excuteSnpeMobilenetSSD(std::unique_ptr<zdl::DlSystem::ITensor>& input,
                    std::vector<std::string>& labels)
{
    //Execute the network and store the outputs that were specified when creating the network in a TensorMap
    static zdl::DlSystem::TensorMap outputTensorMap;
    int exeStatus  = this->snpeRuntine->execute(input.get(), outputTensorMap);
    if(exeStatus == true)
    {
        printf("Execute SNPE successfully \n");
    }
    else
    {
        printf("Error while executing the network \n");
    }
    zdl::DlSystem::StringList tensorNames = outputTensorMap.getTensorNames();
    for(size_t i=0; i<tensorNames.size(); i++) 
    {
        std::cout << tensorNames.at(i) << " (";
        zdl::DlSystem::ITensor *outTensori = outputTensorMap.getTensor(tensorNames.at(i));
        zdl::DlSystem::TensorShape shapei = outTensori->getShape();
        for(size_t j=0; j<shapei.rank(); j++) 
        {
            std::cout << shapei[j] << ",";
        }
        std::cout << ")" << std::endl;
    }
    if(tensorNames.size() != 4) {
        std::cerr << "Output list has " << tensorNames.size() << " elements, while three are expected" << std::endl;
        return ;
    }
    // -----
    size_t imageWidths, imageHeights;
    imageWidths = 300;
    imageHeights = 300;

    std::string scoresName = "Postprocessor/BatchMultiClassNonMaxSuppression_scores";
    std::string classesName = "detection_classes:0";
    std::string boxesName = "Postprocessor/BatchMultiClassNonMaxSuppression_boxes";

    zdl::DlSystem::ITensor *outTensorScores = outputTensorMap.getTensor(scoresName.c_str());
    zdl::DlSystem::ITensor *outTensorClasses = outputTensorMap.getTensor(classesName.c_str());
    zdl::DlSystem::ITensor *outTensorBoxes = outputTensorMap.getTensor(boxesName.c_str());

    zdl::DlSystem::TensorShape scoresShape = outTensorScores->getShape();
    if(scoresShape.rank() != 2) {
        std::cerr << "Scores should have two axis" << std::endl;
        return ;
    }

    const auto *oScores = reinterpret_cast<float *>(&(*outTensorScores->begin()));
    const auto *oClasses = reinterpret_cast<float *>(&(*outTensorClasses->begin()));
    const auto *oBoxes = reinterpret_cast<float *>(&(*outTensorBoxes->begin()));

    std::vector<int> boxes;
    std::vector<int> classes;

    for(size_t curProposal = 0; curProposal < scoresShape[1]; curProposal++) 
    {
        float confidence = oScores[curProposal];
        float label = static_cast<int>(oClasses[curProposal]);
        auto ymin = static_cast<int>(oBoxes[4 * curProposal] * imageHeights);
        auto xmin = static_cast<int>(oBoxes[4 * curProposal + 1] * imageWidths);
        auto ymax = static_cast<int>(oBoxes[4 * curProposal + 2] * imageHeights);
        auto xmax = static_cast<int>(oBoxes[4 * curProposal + 3] * imageWidths);
        if(confidence > 0.6) 
        {
            
        }
    }
}

void SnpeRuntime::excuteSnpeClassifier(std::unique_ptr<zdl::DlSystem::ITensor>& input,
                    std::vector<std::string>& labels, std::vector<ObjectClassifier>& classes)
{
    //Execute the network and store the outputs that were specified when creating the network in a TensorMap
    static zdl::DlSystem::TensorMap outputTensorMap;
    int exeStatus  = this->snpeRuntine->execute(input.get(), outputTensorMap);
    if(exeStatus == true)
    {
        printf("Execute SNPE successfully \n");
    }
    else
    {
        printf("Error while executing the network \n");
    }
    zdl::DlSystem::StringList tensorNames = outputTensorMap.getTensorNames();
    // get class, scores
    zdl::DlSystem::ITensor *outTensor = outputTensorMap.getTensor(tensorNames.at(0));
    float* outData = reinterpret_cast<float*>(&(*outTensor->begin()));
    float scoreMax = 0.0;
    int classID = 0;
    classes.clear();
    for (size_t j = 0; j < outTensor->getSize(); j++) 
    {
        ObjectClassifier obj;
        obj.label = labels[j];
        obj.score = outData[j];
        classes.push_back(obj);
    }
}