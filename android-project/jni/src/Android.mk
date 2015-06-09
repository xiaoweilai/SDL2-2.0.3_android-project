LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL2-2.0.3
FFMPEG_PATH := ../
#GLES_PATH := ../freeglut-gles
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
                    $(LOCAL_PATH)/$(FFMPEG_PATH)/include
 #                   $(LOCAL_PATH)/$(GLES_PATH)/include
                    
LOCAL_LDLIBS := -L$(NDK_PLATFORMS_ROOT)/$(TARGET_PLATFORM)/arch-arm/usr/lib -L$(LOCAL_PATH)/$(FFMPEG_PATH) -lavformat -lavcodec -lavdevice -lavfilter -lavcore -lavutil -lswscale -llog -ljnigraphics -lz -ldl -lgcc

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	main.c

LOCAL_SHARED_LIBRARIES := SDL2 #glut-gles

LOCAL_LDLIBS += -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
