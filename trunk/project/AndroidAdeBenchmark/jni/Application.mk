APP_ABI := armeabi-v7a
APP_STL := gnustl_static
APP_CPPFLAGS += -std=c++11
APP_PLATFORM := android-14

APP_CFLAGS := -DADE_TARGET_TYPE=3 -DADE_BLAS_IMPLEMENTATION=1

ifeq ($(NDK_DEBUG),1)
	APP_OPTIM := debug
	APP_CPPFLAGS += -g3 -D_DEBUG -DADE_TARGET_MODE=1 #ADE_DEBUG
else # release
	APP_CPPFLAGS += -DNDEBUG -g0 -DADE_TARGET_MODE=0 #ADE_RELEASE
	APP_OPTIM := release
endif


#APP_STL := c++_static
#NDK_TOOLCHAIN_VERSION=clang3.4

#APP_CPPFLAGS := -fexceptions -frtti -DCC_ENABLE_CHIPMUNK_INTEGRATION=1 -std=c++11 -fsigned-char
#APP_LDFLAGS := -latomic

#ifeq ($(NDK_DEBUG),1)
#	APP_OPTIM := debug
#	APP_CFLAGS += -lstdc++
#	APP_CPPFLAGS += -DCOCOS2D_DEBUG=1
#else # release
#	APP_CPPFLAGS += -DNDEBUG
#	APP_OPTIM := release
#	APP_CFLAGS += -lstdc++ -g0
#endif