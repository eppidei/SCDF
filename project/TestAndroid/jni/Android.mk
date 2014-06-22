# TODO:
# - customized base trunk path
# - external mk file for library
# - export c includes

APP_LOCAL_PATH := $(call my-dir)
SCDF_TRUNK := /home/athos/SCDF/trunk
SCDF_INCLUDES := $(SCDF_TRUNK)/src/include $(SCDF_TRUNK)/src/android/include
SCDF_SRC := $(SCDF_TRUNK)/src

OSCPACK_PATH := $(SCDF_TRUNK)/lib/oscpack_1_1_0
OSCPACK_INCLUDES := $(OSCPACK_PATH) $(OSCPACK_PATH)/ip $(OSCPACK_PATH)/ip/posix $(OSCPACK_PATH)/osc

include $(CLEAR_VARS)
LOCAL_MODULE 	 := oscpack
LOCAL_PATH 		 := $(OSCPACK_PATH)
LOCAL_C_INCLUDES := $(OSCPACK_INCLUDES)
#$(warning LOCAL_PATH is '$(LOCAL_PATH)')
#$(warning LOCAL_C_INCLUDES is '$(LOCAL_C_INCLUDES)')
#LOCAL_LDLIBS += -landroid -llog
LOCAL_SRC_FILES := 	osc/OscOutboundPacketStream.cpp \
					osc/OscPrintReceivedElements.cpp \
					osc/OscReceivedElements.cpp \
					osc/OscTypes.cpp \
					ip/IpEndpointName.cpp \
					ip/posix/NetworkingUtils.cpp \
					ip/posix/UdpSocket.cpp
					 
include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE 	 := scdf
LOCAL_PATH 		 := $(SCDF_SRC)
LOCAL_C_INCLUDES := $(OSCPACK_INCLUDES) $(SCDF_INCLUDES)
LOCAL_STATIC_LIBRARIES := oscpack
#LOCAL_LDLIBS += -landroid -llog
LOCAL_SRC_FILES := 	Sensor.cpp \
					SensorStandard.cpp \
					android/SensorStandardImplAndroid.cpp \
					SensorAudioInput.cpp \
					android/SensorAudioInputImplAndroid.cpp \
					InitFramework.cpp \
					ThreadUtils.cpp \
					UDPSender.cpp \
					UDPSendersManager.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := scdftest
LOCAL_PATH:= $(APP_LOCAL_PATH)
LOCAL_LDLIBS += -landroid -llog
LOCAL_STATIC_LIBRARIES := scdf
LOCAL_SRC_FILES := scdftest.cpp
include $(BUILD_SHARED_LIBRARY)
