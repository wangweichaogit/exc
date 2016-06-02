# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

SRC_COMLIB_PATH :=../../..

SRC_COMLIB :=$(LOCAL_PATH)/../../../..
INC_DHNTIMER :=$(LOCAL_PATH)/../../../../dhntimer
INC_SYS :=$(LOCAL_PATH)/../../../../sys

include $(CLEAR_VARS)
LOCAL_MODULE    := comLib
LOCAL_CPP_EXTENSION := .cpp

LOCAL_SRC_FILES += $(SRC_COMLIB_PATH)/Logger.cpp \
				   $(SRC_COMLIB_PATH)/Logger_impl.cpp \
				   $(SRC_COMLIB_PATH)/IniFile.cpp \
				   $(SRC_COMLIB_PATH)/DateTime.cpp \
				   $(SRC_COMLIB_PATH)/SysUtil.cpp \
				   $(SRC_COMLIB_PATH)/GMRWLockImp.cpp \
				   $(SRC_COMLIB_PATH)/dhntimer/DHNTimer.cpp \
				   $(SRC_COMLIB_PATH)/dhntimer/DHNTimerTask.cpp \
				   $(SRC_COMLIB_PATH)/dhntimer/Time_Value.cpp \
				   $(SRC_COMLIB_PATH)/sys/getopt.cpp \
				   $(SRC_COMLIB_PATH)/sys/gettimeofday.cpp \
				   $(SRC_COMLIB_PATH)/sys/SystemStat.cpp \
LOCAL_CFLAGS += -D_QN_DEBUG
LOCAL_CFLAGS += -D_ANDROID_
LOCAL_CFLAGS += -D_LINUX_
LOCAL_CFLAGS +=	-D___DEV_DEBUG___=0
LOCAL_CFLAGS +=	-D___FORREDCDN___=0
					
LOCAL_C_INCLUDES += $(SRC_COMLIB)\
					$(INC_DHNTIMER)\
					$(INC_SYS)

LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
										
include $(BUILD_STATIC_LIBRARY)