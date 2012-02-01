LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := test_ion.c

LOCAL_C_INCLUDES := 
#LOCAL_CFLAGS := 

LOCAL_MODULE := test_ion

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
