APP_STL := c++_static
NDK_TOOLCHAIN_VERSION=clang3.4

APP_CPPFLAGS := -fexceptions -frtti -DCC_ENABLE_CHIPMUNK_INTEGRATION=1 -std=c++11 -fsigned-char
APP_LDFLAGS := -latomic
APP_PLATFORM := android-9

APP_DEBUG := $(strip $(NDK_DEBUG))

ifeq ($(NDK_DEBUG),1)
	APP_OPTIM := debug
	APP_CFLAGS += -lstdc++
	APP_CPPFLAGS += -DCOCOS2D_DEBUG=1
else # release
	APP_CPPFLAGS += -DNDEBUG
	APP_OPTIM := release
	APP_CFLAGS += -lstdc++ -g0
endif

#ifeq ($(APP_DEBUG),1)
# APP_CPPFLAGS += -DCOCOS2D_DEBUG=1 
#  APP_OPTIM := debug
#else
#  APP_CPPFLAGS += -DNDEBUG
#  APP_OPTIM := release
#endif