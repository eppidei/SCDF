JNI_DIR := $(call my-dir)
FFTW3_VERSION = 3.3.4
FFTW3_BASE_PATH := $(JNI_DIR)/../fftw-$(FFTW3_VERSION)

# --enable-float generates a float-based fftw
# --enable-long-double generates a long double based fft
# NB: neon only works with --enable-float (not with double neither with long double!)
# don't forget --with-slow-timer or the plan estimation will hang
# use --host=arm-eabi to stay safe (things will be set up knowing that target platform is arm)

FFTW3_CONFIG_FLAGS := --with-slow-timer --host=arm-eabi

ifeq ($(FFTW3_PRECISION),double)
 # nothing, means double precision
else 
 ifeq ($(FFTW3_PRECISION),long)
  FFTW3_CONFIG_FLAGS += --enable-long-double
 else
  FFTW3_PRECISION:=float 
  # NB - float is the default, so we explicitly set it here in case it wasn't
  # so we can check against this in the neon enable if
  FFTW3_CONFIG_FLAGS += --enable-float
 endif
endif

ifeq ($(FFTW3_ENABLE_NEON),true)
 FFTW3_CONFIG_FLAGS +=--enable-neon
 #TODO - decide if you want to enable neon capable build here or only from outside
 ifeq ($(FFTW3_PRECISION),double)
   $(error "Can't use DOUBLE fftw! NEON needs FLOAT precision")
 endif
 ifeq ($(FFTW3_PRECISION),long)
   $(error "Can't use LONG DOUBLE fftw! NEON needs FLOAT precision")
 endif
endif

RES := $(shell ($(JNI_DIR)/config-android.sh $(FFTW3_VERSION) $(FFTW3_CONFIG_FLAGS) ) )

include $(CLEAR_VARS)

LOCAL_MODULE := fftw3
LOCAL_PATH   := $(FFTW3_BASE_PATH)
include $(JNI_DIR)/src.mk
include $(JNI_DIR)/inc.mk
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

ifeq ($(FFTW3_BUILD_SHARED),true)
 include $(BUILD_SHARED_LIBRARY)
else
 include $(BUILD_STATIC_LIBRARY)
endif


# Threads lib
#include $(CLEAR_VARS)
#LOCAL_MODULE    := fftw_threads
#include $(LOCAL_PATH)/threads_src.mk
#include $(LOCAL_PATH)/inc.mk
#include $(BUILD_STATIC_LIBRARY)
