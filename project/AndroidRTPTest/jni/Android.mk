LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := RTPTest
LOCAL_SRC_FILES := RTPTest.cpp

include $(BUILD_SHARED_LIBRARY)
