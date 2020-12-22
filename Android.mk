# Copyright (C) 2020, LE MANH THANG. All rights reserved.
# Module: Android.mk
# Author: LE MANH THANG
# Created: 21/12/2020
# Description: 

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
# inlcude linopencv-android-armv7a
LOCAL_C_INCLUDES := $(LOCAL_PATH)/libopencv-4.5.0-android-armv7a-prebuilt/sdk/native/jni/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/libSNPE-prebuilt/include/zdl

# share lib
LOCAL_SHARED_LIBRARIES := libopencv_world-prebuilt
LOCAL_SHARED_LIBRARIES += libsnpe-prebuilt

LOCAL_MODULE := snpe
LOCAL_SRC_FILES := \
				src/Util.cpp \
				src/ImageProcessing.cpp \
				src/SnpeRuntime.cpp \
				src/HatClassifier.cpp \
				src/main.cpp \

LOCAL_CFLAGS += -fopenmp -fPIE -fPIC 
LOCAL_CPPFLAGS += -fopenmp -fPIE -fPIC 
LOCAL_LDFLAGS += -fopenmp  -fPIE -fPIC -Wl,--gc-sections 
LOCAL_LDLIBS := -lz -llog -ljnigraphics -landroid -lGLESv2 -lEGL

include $(BUILD_EXECUTABLE)
#include $(BUILD_SHARED_LIBRARY)


#add lib opencv prebuilt
include $(CLEAR_VARS)
LOCAL_MODULE := libopencv_world-prebuilt
LOCAL_SRC_FILES := $(LOCAL_PATH)/libopencv-4.5.0-android-armv7a-prebuilt/sdk/native/libs/armeabi-v7a/libopencv_world.so
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/libopencv-4.5.0-android-armv7a-prebuilt/sdk/native/jni/include
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)

#add lib snpe prebuilt
include $(CLEAR_VARS)
LOCAL_MODULE := libsnpe-prebuilt
LOCAL_SRC_FILES := $(LOCAL_PATH)/libSNPE-prebuilt/lib/arm-android-clang6.0/libSNPE.so
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/jni/libSNPE-prebuilt/include/zdl
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)
