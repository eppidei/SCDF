SCDF_CTRL_LOCAL_PATH := $(call my-dir)

SCDF_SRC_PATH := /home/athos/SCDF/trunk/src/
LIBUSB_DIR := /home/athos/SCDF/trunk/lib/LibUsbCustom
LIBUSB_SRC := $(LIBUSB_DIR)/libusb
LIBUSB_INCLUDES := $(LIBUSB_SRC) $(LIBUSB_SRC)/os $(LIBUSB_SRC)/android


# SCDF:

SCDF_TRUNK := /home/athos/SCDF/trunk
SCDF_INCLUDES := $(SCDF_TRUNK)/src/include \
				$(SCDF_TRUNK)/src/android/include \
				$(SCDF_TRUNK)/lib/cereal-1.1.0/include
					
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
LOCAL_MODULE := usbcustom
LOCAL_PATH:=$(LIBUSB_SRC)
LOCAL_C_INCLUDES := $(LIBUSB_INCLUDES)
LOCAL_SRC_FILES := core.c descriptor.c io.c sync.c os/linux_usbfs.c os/threads_posix.c
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE 	 := scdf
LOCAL_PATH 		 := $(SCDF_SRC)
LOCAL_C_INCLUDES := $(OSCPACK_INCLUDES) $(SCDF_INCLUDES) $(LIBUSB_INCLUDES)
LOCAL_EXPORT_C_INCLUDES := $(OSCPACK_INCLUDES) $(SCDF_INCLUDES)
#LOCAL_CFLAGS := -DADE_TARGET_TYPE=3 #ADE_ANDROID 
LOCAL_STATIC_LIBRARIES := oscpack 
LOCAL_SHARED_LIBRARIES += usbcustom
LOCAL_LDLIBS += -landroid -llog -lOpenSLES
LOCAL_SRC_FILES := 	Sensor.cpp \
					SensorStandard.cpp \
					android/SensorStandardImplAndroid.cpp \
					SensorAudioInput.cpp \
					android/SensorAudioInputImplAndroid.cpp \
					SensorsManager.cpp \
					CustomPipe.cpp \
					ThreadUtils.cpp \
					UDPSender.cpp \
					UDPSendersManager.cpp \
					Harvester.cpp \
					PipesManager.cpp \
					android/FrameworkJniGlue.cpp \
					android/SaveSettings.cpp \
					android/OsUtilitiesAndroid.cpp \
					ScdfSensorAPI.cpp \
					android/JniGlue.cpp \
					android/UsbHandler.cpp \
					android/usb/UsbHelper.cpp \
					android/usb/UsbAudioEndpoint.cpp \
					android/usb/UsbAudioInterfaces.cpp \
					android/usb/UsbMidiInterface.cpp \
					android/usb/UsbAudioDevice.cpp \
					android/MidiOutConnectionAndroid.cpp \
					MidiOutConnection.cpp \
					OsUtilities.cpp
					#InitFramework.cpp \
					
					
include $(BUILD_SHARED_LIBRARY)

# USB AND COCOS2D APP:

#include $(CLEAR_VARS)
#LOCAL_MODULE    := scdfusb
#LOCAL_PATH := $(SCDF_SRC_PATH)/android
#LOCAL_C_INCLUDES += $(SCDF_SRC_PATH)/android/include \
					$(SCDF_SRC_PATH)/include \
					$(LIBUSB_INCLUDES)
					 
#LOCAL_SHARED_LIBRARIES += usbcustom
#LOCAL_LDLIBS += -landroid -llog
#LOCAL_SRC_FILES := 	JniGlue.cpp \
					UsbHandler.cpp \
					usb/UsbHelper.cpp \
					usb/UsbAudioEndpoint.cpp \
					usb/UsbAudioInterfaces.cpp \
					usb/UsbMidiInterface.cpp \
					usb/UsbAudioDevice.cpp \
					MidiOutConnectionAndroid.cpp

#include $(BUILD_SHARED_LIBRARY)

BLAS_MAKEFILE_PATH:=$(SCDF_TRUNK)/lib/CLAPACK/jni
FFTW3_MAKEFILE_PATH:=$(SCDF_TRUNK)/lib/FFTw3/jni
ADE_SRC_PATH:=$(SCDF_TRUNK)/ext/ADE_API/API/src

#BLAS_BUILD_SHARED:=true # used inside blas makefile
include $(BLAS_MAKEFILE_PATH)/Android.mk

#FFTW3_BUILD_SHARED:=true # used inside fftw3 makefile
include $(FFTW3_MAKEFILE_PATH)/Android.mk

include $(CLEAR_VARS)
LOCAL_PATH := $(ADE_SRC_PATH)
LOCAL_C_INCLUDES := $(ADE_SRC_PATH) $(ADE_SRC_PATH)/headers
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)
LOCAL_SHARED_LIBRARIES := blas fftw3 fftw_threads
#LOCAL_CFLAGS := -DADE_TARGET_TYPE=3 #ADE_ANDROID 
#-DADE_BLAS_IMPLEMENTATION=1 -DADE_FP_PRECISION=1
LOCAL_MODULE    := ade
LOCAL_SRC_FILES :=  ADE.c \
					ADE_Bench_Utils.c \
					ADE_blas_level1.c \
					ADE_blas_level2.c \
					ADE_blas_level3.c \
					ADE_complex.c \
					ADE_Utils.c \
					ADE_Blow.c \
					ADE_iir.c \
					ADE_fir.c \
					ADE_polyfit.c \
					ADE_Error_Handler.c \
					ADE_Snap.c \
					ADE_fft.c \
					ADE_nrutil.c			
include $(BUILD_SHARED_LIBRARY)




include $(CLEAR_VARS)
LOCAL_PATH := $(SCDF_CTRL_LOCAL_PATH)

# NB: to solve a bug with ndk r10b, needed to solve a bug with c++_static
# needed to use Cereal (gnustl doesnt have stoi & co), we modified
# cocos2d/extensions/android.mk, to remove dependency from Curl. We had to
# remove AssetsManager.cpp

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)

LOCAL_MODULE := cocos2dcpp_shared
LOCAL_MODULE_FILENAME := libcocos2dcpp
#LOCAL_CFLAGS := -DADE_TARGET_TYPE=3 #ADE_ANDROID 
LOCAL_SRC_FILES := hellocpp/main.cpp \
        ../../Classes/AppDelegate.cpp \
        ../../Classes/MainScene.cpp \
		../../Classes/SCDFCItems.cpp \
		../../Classes/SCDFCScrollView.cpp \
		../../Classes/SCDFCWorkingPanel.cpp \
		../../Classes/PropertiesPanel.cpp \
		../../Classes/MultiSender.cpp \
		../../Classes/DropDownMenu.cpp \
		../../Classes/ControlUnitPatch.cpp \
		../../Classes/Colors.cpp \
		../../Classes/Configuration.cpp \
		../../Classes/LoadSavePanel.cpp \
		../../Classes/Panel.cpp \
		../../Classes/ControlUnit.cpp \
		../../Classes/Label.cpp \
		../../Classes/android/InAppAndroid.cpp
		
LOCAL_LDLIBS += -landroid -llog

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../Classes \
					$(SCDF_SRC_PATH)/android/include \
					$(SCDF_SRC_PATH)/include \
					$(LIBUSB_INCLUDES)\
					$(SCDF_TRUNK)/lib/cereal-1.1.0/include \
					$(OSCPACK_PATH)

LOCAL_SHARED_LIBRARIES := usbcustom scdfusb scdf
LOCAL_SHARED_LIBRARIES += ade

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
#LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static

# LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocosbuilder_static
# LOCAL_WHOLE_STATIC_LIBRARIES += spine_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocostudio_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocos_network_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static
include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module,audio/android)

# $(call import-module,Box2D)
# $(call import-module,editor-support/cocosbuilder)
# $(call import-module,editor-support/spine)
$(call import-module,editor-support/cocostudio)
# $(call import-module,network)
$(call import-module,extensions)

