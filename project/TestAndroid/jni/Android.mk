APP_LOCAL_PATH := $(call my-dir)
SCDF_TRUNK := /home/athos/SCDF/trunk
SCDF_INCLUDE := $(SCDF_TRUNK)/src/include
SCDF_SRC := $(SCDF_TRUNK)/src

include $(CLEAR_VARS)
LOCAL_PATH := $(SCDF_SRC)
LOCAL_MODULE := scdf
LOCAL_C_INCLUDES := $(SCDF_INCLUDE)
#LOCAL_LDLIBS += -landroid -llog
LOCAL_SRC_FILES := Sensor.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := scdftest
LOCAL_PATH:= $(APP_LOCAL_PATH)
LOCAL_LDLIBS += -landroid -llog
LOCAL_STATIC_LIBRARIES := scdf
LOCAL_SRC_FILES := scdftest.cpp
include $(BUILD_SHARED_LIBRARY)
