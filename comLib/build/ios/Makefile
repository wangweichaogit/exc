# comLib Makefile

vpath %.c   $(SRCPATH)
vpath %.cc  $(SRCPATH)
vpath %.cpp $(SRCPATH)
vpath %.m   $(SRCPATH)
vpath %.h   $(SRCPATH)

SRCPATH := ../..

SDKVERSION := 9.2
#ARCH := armv7
ifeq ($(ARCH), i386)
    PLATFORM := iPhoneSimulator
    EXTRA_FLAGS := -fPIC -arch i386
endif
ifeq ($(ARCH), armv7)
    PLATFORM := iPhoneOS
    EXTRA_FLAGS := -fPIC -mcpu=cortex-a8 -arch armv7
endif
ifeq ($(ARCH), armv7s)
    PLATFORM := iPhoneOS
    EXTRA_FLAGS := -fPIC -mcpu=cortex-a8 -arch armv7s
endif
ifeq ($(ARCH),arm64)
	PLATFORM := iPhoneOS
	EXTRA_FLAGS := -fPIC -arch arm64
endif

CC := /Applications/Xcode.app/Contents/Developer/usr/bin/gcc
CXX := /Applications/Xcode.app/Contents/Developer/usr/bin/g++
AR := ar rc
RANLIB := ranlib
SYSROOT := /Applications/Xcode.app/Contents/Developer/Platforms/$(PLATFORM).platform/Developer/SDKs/$(PLATFORM)$(SDKVERSION).sdk

ifeq ($(SYSROOT), $(wildcard $(SYSROOT)))
else
SDKVERSION := 7.1
SYSROOT := /Applications/Xcode.app/Contents/Developer/Platforms/$(PLATFORM).platform/Developer/SDKs/$(PLATFORM)$(SDKVERSION).sdk
endif

CPPFLAGS := $(SYSROOT) _D_FILE_OFFSET_BITS=64 -DPIC
INCS := -I$(SYSROOT)/usr/include/
$(EXTRA_FLAGS) := -D_FILE_OFFSET_BITS=64 -DPIC $(EXTRA_FLAGS)

LIBCOMLIB := build/$(ARCH)/libcomLib.a

all: $(LIBCOMLIB)

define COMPILE
    $($(1)) -c -g $< -isysroot $(SYSROOT) $($(1)FLAGS) $(EXTRA_FLAGS) -o $@
endef

%.o: %.c
	$(call COMPILE,CC)
%.o: %.cpp
	$(call COMPILE,CXX)
%.o: %.cc
	$(call COMPILE,CXX)
%.o: %.m
	$(CC) -x objective-c -c -g $< -isysroot $(SYSROOT) $(CCFLAGS) $(EXTRA_FLAGS) -o $@

### comLib ###
SRCS := $(SRCPATH)/DateTime.cpp \
        $(SRCPATH)/IniFile.cpp \
        $(SRCPATH)/Logger_impl.cpp \
        $(SRCPATH)/Logger.cpp \
        $(SRCPATH)/SysUtil.cpp \
        $(SRCPATH)/GMRWLockImp.cpp \
        $(SRCPATH)/LogHelper.cpp \
        $(SRCPATH)/dhntimer/DHNTimer.cpp \
        $(SRCPATH)/dhntimer/DHNTimerTask.cpp \
        $(SRCPATH)/dhntimer/Time_Value.cpp \
        $(SRCPATH)/sys/gettimeofday.cpp \
        $(SRCPATH)/sys/getopt.cpp \
        $(SRCPATH)/sys/cJSON.cpp \
        $(SRCPATH)/sys/SystemStat.cpp

OBJS_comlib := $(SRCS:%.cpp=%.o)

$(OBJS_comlib): include_dirs := -I$(SRCPATH) -I$(SRCPATH)/../
$(OBJS_comlib): CFLAGS := $(include_dirs) \
                -D__IOS__ \
                -D_QN_DEBUG
                
$(OBJS_comlib): CCFLAGS := $(CFLAGS)
$(OBJS_comlib): CXXFLAGS := $(CCFLAGS)

### libcomlib ###
OBJS := $(OBJS_comlib)
OBJS := $(filter %.o,$(OBJS))

$(LIBCOMLIB) : $(OBJS)
	@mkdir -p build/$(ARCH)
	@rm -f $(LIBCOMLIB)
	$(AR) $@ $(OBJS)
	$(if $(RANLIB), $(RANLIB) $@)

.PHONY: all clean cleanall

clean:
	rm -f $(OBJS)

cleanall:
	rm -f $(OBJS) $(LIBCOMLIB)
