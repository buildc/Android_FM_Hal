LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	alex_eport_hal_test.cpp

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../../device/dli/duv060/libhardware/include

LOCAL_SHARED_LIBRARIES := \
	libcutils libhardware

LOCAL_MODULE:= alex_eport_hal_test

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)
LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_SBIN_UNSTRIPPED)

include $(BUILD_EXECUTABLE)
