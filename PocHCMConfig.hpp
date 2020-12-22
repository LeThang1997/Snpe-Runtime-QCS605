
/******************************************************************************** 
 Copyright (C) 2020, LE MANH THANG. All rights reserved.
 Module: SnpeRuntime.cpp
 Author: LE MANH THANG
 Created: 21/12/2020
 Description: 
********************************************************************************/
#ifndef POCHCMCONFIG_H
#define POCHCMCONFIG_H

#define PATH_MODEL_HAT "/data/bkavai/snpe/models/hat_quantized.dlc"
#define PATH_LABEL_HAT "/data/bkavai/snpe/models/hat_labels.txt"

#define PATH_MODEL_COLORFASHION "/data/bkavai/snpe/models/fashion.dlc"
#define PATH_LABEL_COLORFASHION "/data/bkavai/snpe/models/colorfashion_labels.txt"


struct ObjectClassifier
{
    std::string label;
    float score;
};
#endif