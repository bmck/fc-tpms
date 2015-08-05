# $Id$
# $(c)$

LOCAL_PATH:= $(call my-dir)

#
# FleetCents proprietary library
#
include $(CLEAR_VARS)
LOCAL_MODULE    := fleetcents
LOCAL_SRC_FILES := \
  fleetcents/fc_jni_harness.c \
  fleetcents/liquid_analysis.c
LOCAL_LDLIBS := -lm -llog
LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/fleetcents 
include $(BUILD_SHARED_LIBRARY)

#
# RTL-TCP for Android per https://github.com/martinmarinov/rtl_tcp_andro-
#
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
 rtl_tcp_android/libusb-andro/libusb/core.c \
 rtl_tcp_android/libusb-andro/libusb/descriptor.c \
 rtl_tcp_android/libusb-andro/libusb/hotplug.c \
 rtl_tcp_android/libusb-andro/libusb/io.c \
 rtl_tcp_android/libusb-andro/libusb/sync.c \
 rtl_tcp_android/libusb-andro/libusb/strerror.c \
 rtl_tcp_android/libusb-andro/libusb/os/linux_usbfs.c \
 rtl_tcp_android/libusb-andro/libusb/os/poll_posix.c \
 rtl_tcp_android/libusb-andro/libusb/os/threads_posix.c \
 rtl_tcp_android/libusb-andro/libusb/os/linux_netlink.c \
 rtl_tcp_android/rtl-sdr/src/convenience/convenience.c \
 rtl_tcp_android/librtlsdr_andro.c \
 rtl_tcp_android/rtl_sdr_andro.c \
 rtl_tcp_android/RtlSdr.c \
 rtl_tcp_android/rtl-sdr/src/tuner_e4k.c \
 rtl_tcp_android/rtl-sdr/src/tuner_fc0012.c \
 rtl_tcp_android/rtl-sdr/src/tuner_fc0013.c \
 rtl_tcp_android/rtl-sdr/src/tuner_fc2580.c \
 rtl_tcp_android/rtl-sdr/src/tuner_r82xx.c
LOCAL_C_INCLUDES += \
 $(LOCAL_PATH)/rtl_tcp_android \
 $(LOCAL_PATH)/rtl_tcp_android/libusb-andro \
 $(LOCAL_PATH)/rtl_tcp_android/libusb-andro/libusb \
 $(LOCAL_PATH)/rtl_tcp_android/libusb-andro/libusb/os \
 $(LOCAL_PATH)/rtl_tcp_android/rtl-sdr/include \
 $(LOCAL_PATH)/rtl_tcp_android/rtl-sdr/src \
 $(LOCAL_PATH)/fleetcents
LOCAL_CFLAGS += -Wall -DLIBUSB_DESCRIBE="" -O3 -fno-builtin-printf -fno-builtin-fprintf
LOCAL_MODULE:= rtlsdr
LOCAL_LDLIBS := -lm -llog
LOCAL_SHARED_LIBRARIES :=  fleetcents
include $(BUILD_SHARED_LIBRARY)
