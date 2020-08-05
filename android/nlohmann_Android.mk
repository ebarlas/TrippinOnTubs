LOCAL_PATH :=  $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := nlohmann_json

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_EXPORT_C_INCLUDES += $(LOCAL_C_INCLUDES)