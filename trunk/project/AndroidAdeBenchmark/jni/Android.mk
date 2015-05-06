LOCAL_PATH := $(call my-dir)
ADE_TEST_JNI_DIR := $(LOCAL_PATH)

SCDF_TRUNK:=/home/athos/SCDF/trunk
BLAS_MAKEFILE_PATH:=$(SCDF_TRUNK)/lib/CLAPACK/jni
FFTW3_MAKEFILE_PATH:=$(SCDF_TRUNK)/lib/FFTw3/jni
ADE_SRC_PATH:=$(SCDF_TRUNK)/../API/src

BLAS_BUILD_SHARED:=false
include $(BLAS_MAKEFILE_PATH)/Android.mk

FFTW3_BUILD_SHARED:=false
include $(FFTW3_MAKEFILE_PATH)/Android.mk

include $(CLEAR_VARS)
LOCAL_PATH := $(ADE_SRC_PATH)
LOCAL_C_INCLUDES := $(ADE_SRC_PATH) $(ADE_SRC_PATH)/headers
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)
LOCAL_SHARED_LIBRARIES := blas fftw3
#LOCAL_CFLAGS := -DADE_BLAS_IMPLEMENTATION=1 -DADE_FP_PRECISION=1
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
					ADE_polyfit.c					
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_PATH := $(ADE_TEST_JNI_DIR)
LOCAL_STATIC_LIBRARIES := ade
LOCAL_LDLIBS := -llog
LOCAL_MODULE    := adebenchmark
LOCAL_SRC_FILES := adebenchmark.cpp
include $(BUILD_SHARED_LIBRARY)
