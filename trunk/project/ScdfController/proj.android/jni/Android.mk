SCDF_CTRL_LOCAL_PATH := $(call my-dir)

SCDF_SRC_PATH := /home/athos/SCDF/trunk/src/
LIBUSB_DIR := /home/athos/SCDF/trunk/lib/LibUsbCustom
LIBUSB_SRC := $(LIBUSB_DIR)/libusb
LIBUSB_INCLUDES := $(LIBUSB_SRC) $(LIBUSB_SRC)/os $(LIBUSB_SRC)/android

include $(CLEAR_VARS)
LOCAL_MODULE := usbcustom
LOCAL_PATH:=$(LIBUSB_SRC)
LOCAL_C_INCLUDES := $(LIBUSB_INCLUDES)
LOCAL_SRC_FILES := core.c descriptor.c io.c sync.c os/linux_usbfs.c os/threads_posix.c
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := scdfusbtest
LOCAL_PATH := $(SCDF_SRC_PATH)/android
LOCAL_C_INCLUDES += $(SCDF_SRC_PATH)/android/include $(SCDF_SRC_PATH)/include $(LIBUSB_INCLUDES)
LOCAL_SHARED_LIBRARIES += usbcustom
LOCAL_LDLIBS += -landroid -llog
LOCAL_SRC_FILES := 	JniGlue.cpp \
					UsbHandler.cpp \
					usb/UsbHelper.cpp \
					usb/UsbAudioEndpoint.cpp \
					usb/UsbAudioInterfaces.cpp \
					usb/UsbMidiInterface.cpp \
					usb/UsbAudioDevice.cpp \
					MidiOutConnectionAndroid.cpp

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_PATH := $(SCDF_CTRL_LOCAL_PATH)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../Classes/AppDelegate.cpp \
                   ../../Classes/MainScene.cpp \
		../../Classes/SCDFCItems.cpp \
		../../Classes/SCDFCScrollView.cpp \
		../../Classes/SCDFCWorkingPanel.cpp \
		../../Classes/PropertiesPanel.cpp \
		../../Classes/ControlUnit.cpp

LOCAL_LDLIBS += -landroid -llog

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes $(SCDF_SRC_PATH)/android/include $(SCDF_SRC_PATH)/include $(LIBUSB_INCLUDES)

LOCAL_SHARED_LIBRARIES := usbcustom scdfusbtest

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static

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
