LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL2_PATH := ../SDL2-2.0.12
SDL2_IMAGE_PATH := ../SDL2_image-2.0.5
SDL2_MIXER_PATH := ../SDL2_mixer-2.0.4
NLOHMANN_JSON_PATH := ../json-3.8.0

LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/$(SDL2_PATH)/include $(LOCAL_PATH)/$(SDL2_IMAGE_PATH)/include $(LOCAL_PATH)/$(SDL2_MIXER_PATH)/include $(LOCAL_PATH)/$(NLOHMANN_JSON_PATH)/include

LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/engine/*.cpp) $(wildcard $(LOCAL_PATH)/game/*.cpp) $(wildcard $(LOCAL_PATH)/lock/*.cpp) $(wildcard $(LOCAL_PATH)/sprite/*.cpp)

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_mixer

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
