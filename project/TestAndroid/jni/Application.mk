APP_ABI := armeabi-v7a 
APP_PLATFORM := android-10
#APP_STL := stlport_static
APP_STL := gnustl_static
APP_GNUSTL_FORCE_CPP_FEATURES := exceptions rtti

ifeq ($(NDK_DEBUG),1)
APP_OPTIM := debug
APP_CFLAGS += -lstdc++
else # release
APP_OPTIM := release
APP_CFLAGS += -lstdc++ -g0
endif