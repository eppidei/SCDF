MY_PATH := $(call my-dir)
SCDF_SRC_PATH := $(MY_PATH)/../../../src/

# ----------------------- LIBUSB -------------------------#

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
