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
LOCAL_MODULE    := "fleetcents"
LOCAL_SRC_FILES := fleetcents/rtlsdr-to-cfile2.c fleetcents/rtlsdr_cplx-to-pkt.c fleetcents/rtlsdr_harness.c
LOCAL_C_INCLUDES += $(LOCAL_PATH)/android/include
LOCAL_LDLIBS :=  -llog
include $(BUILD_SHARED_LIBRARY)

