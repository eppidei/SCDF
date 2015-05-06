APP_ABI := armeabi-v7a
APP_STL := c++_static
NDK_TOOLCHAIN_VERSION=clang3.4

APP_CPPFLAGS := -fexceptions -frtti -DCC_ENABLE_CHIPMUNK_INTEGRATION=1 -std=c++11 -fsigned-char 
APP_CFLAGS := -DADE_TARGET_TYPE=3 -DADE_BLAS_IMPLEMENTATION=1
			#     ADE_ANDROID 			# CBLAS
APP_LDFLAGS := -latomic -lstdc++
APP_PLATFORM := android-14

#APP_DEBUG := $(strip $(NDK_DEBUG))

ifeq ($(NDK_DEBUG),1)
	APP_OPTIM := debug
	APP_CPPFLAGS += -DCOCOS2D_DEBUG=1 -g3 -D_DEBUG
else # release
	APP_CPPFLAGS += -DNDEBUG -g0
	APP_OPTIM := release
endif

#ifeq ($(APP_DEBUG),1)
# APP_CPPFLAGS += -DCOCOS2D_DEBUG=1 
#  APP_OPTIM := debug
#else
#  APP_CPPFLAGS += -DNDEBUG
#  APP_OPTIM := release
#endif