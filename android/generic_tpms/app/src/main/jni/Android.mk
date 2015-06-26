# $Id$
# #(c)$
#
# Portions of this file are part of rtl-sdr, and are Copyright 2012 OSMOCOM Project
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

LOCAL_PATH := $(call my-dir)
TARGET_PLATFORM:= android-18

#
# FleetCents proprietary library
#
include $(CLEAR_VARS)
LOCAL_MODULE    := fleetcents
LOCAL_SRC_FILES := fleetcents/rtlsdr_cplx-to-pkt.c \
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
 rtl_tcp_android/rtl-sdr/src/tuner_e4k.c \
 rtl_tcp_android/rtl-sdr/src/tuner_fc0012.c \
 rtl_tcp_android/rtl-sdr/src/tuner_fc0013.c \
 rtl_tcp_android/rtl-sdr/src/tuner_fc2580.c \
 rtl_tcp_android/rtl-sdr/src/tuner_r82xx.c \
 rtl_tcp_android/librtlsdr_andro.c \
 rtl_tcp_android/RtlTcp.c \
 rtl_tcp_android/rtl_tcp_andro.c

LOCAL_CFLAGS += -Wall -DLIBUSB_DESCRIBE="" -O3 -fno-builtin-printf -fno-builtin-fprintf
LOCAL_LDLIBS := -lm -llog
LOCAL_C_INCLUDES := \
 rtl_tcp_android/libusb-andro \
 rtl_tcp_android/libusb-andro/libusb \
 rtl_tcp_android/libusb-andro/libusb/os \
 rtl_tcp_android/rtl-sdr/include \
 rtl_tcp_android/rtl-sdr/src \
 rtl_tcp_android

include $(BUILD_SHARED_LIBRARY)

