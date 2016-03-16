mDNS_TEST_JNI_DIR := $(call my-dir)
#SCDF_TRUNK := /Users/leonardo/Documents/SCDF2.0
SCDF_TRUNK := $(mDNS_TEST_JNI_DIR)/../../../..

BLAS_MAKEFILE_PATH:=$(SCDF_TRUNK)/lib/CLAPACK/jni
FFTW3_MAKEFILE_PATH:=$(SCDF_TRUNK)/lib/FFTw3/jni
mDNS_SRC_PATH:=$(SCDF_TRUNK)/project/test_rtp_session/test_mdsn/mDNSResponder-576.30.4

#include $(BLAS_MAKEFILE_PATH)/Android.mk
#include $(FFTW3_MAKEFILE_PATH)/Android.mk

include $(CLEAR_VARS)
LOCAL_PATH := $(mDNS_SRC_PATH)
LOCAL_C_INCLUDES := $(mDNS_SRC_PATH) $(mDNS_SRC_PATH)/mDNSCore $(mDNS_SRC_PATH)/mDNSPosix $(mDNS_SRC_PATH)/mDNSShared
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)
LOCAL_SHARED_LIBRARIES := blas fftw3 fftw_threads
LOCAL_CFLAGS := -DNOT_HAVE_SA_LEN -DUSES_NETLINK -DDNSSEC_DISABLED -DMDNS_DEBUGMSGS
#-DmDNS_BLAS_IMPLEMENTATION=1 -DmDNS_FP_PRECISION=1
LOCAL_MODULE    := mDNS
LOCAL_SRC_FILES :=  ./mDNSCore/CryptoAlg.c ./mDNSCore/DNSCommon.c ./mDNSCore/DNSDigest.c ./mDNSCore/anonymous.c \
./mDNSCore/mDNS.c ./mDNSCore/nsec3.c \
./mDNSCore/uDNS.c \
./mDNSPosix/ExampleClientApp.c \
./mDNSPosix/mDNSPosix.c \
./mDNSPosix/mDNSUNP.c \
./mDNSShared/DebugServices.c \
./mDNSShared/GenLinkedList.c \
./mDNSShared/PlatformCommon.c \
./mDNSShared/dnssd_clientlib.c \
./mDNSShared/mDNSDebug.c 

					
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_PATH := $(mDNS_TEST_JNI_DIR)
LOCAL_STATIC_LIBRARIES := mDNS
LOCAL_LDLIBS := -llog
LOCAL_MODULE    := adebenchmark
LOCAL_SRC_FILES := adebenchmark.cpp
include $(BUILD_SHARED_LIBRARY)
