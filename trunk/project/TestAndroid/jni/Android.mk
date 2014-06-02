LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_LDLIBS += -landroid -llog
LOCAL_MODULE    := scdftest
LOCAL_SRC_FILES := scdftest.cpp

include $(BUILD_SHARED_LIBRARY)
