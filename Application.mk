# Copyright (C) 2020, LE MANH THANG. All rights reserved.
# Module: Application.mk
# Author: LE MANH THANG
# Created: 21/12/2020
# Description: 
APP_STL := c++_static
APP_STL := c++_shared
APP_ABI := armeabi-v7a
APP_PLATFORM := android-24
APP_CPPFLAGS := -std=c++11 -fexceptions -frtti
APP_LDFLAGS = -lc -lm -ldl -lgcc
