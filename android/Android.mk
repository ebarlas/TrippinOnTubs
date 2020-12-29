LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL2_PATH := ../SDL2-2.0.12/
SDL2_IMAGE_PATH := ../SDL2_image-2.0.5/
SDL2_TTF_PATH := ../SDL2_ttf-2.0.15/
NLOHMANN_JSON_PATH := ../json-3.8.0/

LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/$(SDL2_PATH)/include $(LOCAL_PATH)/$(SDL2_IMAGE_PATH)/include $(LOCAL_PATH)/$(SDL2_TTF_PATH)/include $(LOCAL_PATH)/$(NLOHMANN_JSON_PATH)/include

LOCAL_SRC_FILES := applications/tgoggin.cpp applications/gameloop.cpp $(wildcard $(LOCAL_PATH)/engine/*.cpp) $(wildcard $(LOCAL_PATH)/exception/*.cpp) $(wildcard $(LOCAL_PATH)/graphics/*.cpp) $(wildcard $(LOCAL_PATH)/io/*.cpp) $(wildcard $(LOCAL_PATH)/digits/*.cpp)

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_ttf

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
