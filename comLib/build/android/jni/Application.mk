#APP_STL := stlport_static

APP_STL := gnustl_static
APP_CPPFLAGS += -fexceptions
APP_CPPFLAGS += -frtti
APP_ABI := armeabi-v7a
#APP_ABI := armeabi armeabi-v7a
APP_MODULES := comLib
#APP_OPTIM:= debug
#APP_OPTIM:= release
#APP_CFLAGS
#APP_MODULES := lib库名    如：qnSipPhoneSdk   编译静态库的时候使用 
