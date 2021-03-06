# $Id$
# $(c)$

LOCAL_PATH:= $(call my-dir)

#
# Liquid DSP
#
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
  liquid-dsp/src/agc/src/agc_crcf.c \
  liquid-dsp/src/agc/src/agc_rrrf.c \
  liquid-dsp/src/audio/src/cvsd.c \
  liquid-dsp/src/buffer/src/buffercf.c \
  liquid-dsp/src/buffer/src/bufferf.c \
  liquid-dsp/src/dotprod/src/dotprod_cccf.c \
  liquid-dsp/src/dotprod/src/dotprod_crcf.c \
  liquid-dsp/src/dotprod/src/dotprod_rrrf.c \
  liquid-dsp/src/dotprod/src/sumsq.c \
  liquid-dsp/src/equalization/src/equalizer_cccf.c \
  liquid-dsp/src/equalization/src/equalizer_rrrf.c \
  liquid-dsp/src/fec/src/c_ones_mod2.c \
  liquid-dsp/src/fec/src/c_ones_mod2_gentab.c \
  liquid-dsp/src/fec/src/crc.c \
  liquid-dsp/src/fec/src/fec.c \
  liquid-dsp/src/fec/src/fec_conv.c \
  liquid-dsp/src/fec/src/fec_conv_pmatrix.c \
  liquid-dsp/src/fec/src/fec_conv_poly.c \
  liquid-dsp/src/fec/src/fec_conv_punctured.c \
  liquid-dsp/src/fec/src/fec_golay2412.c \
  liquid-dsp/src/fec/src/fec_hamming128.c \
  liquid-dsp/src/fec/src/fec_hamming128_gentab.c \
  liquid-dsp/src/fec/src/fec_hamming1511.c \
  liquid-dsp/src/fec/src/fec_hamming3126.c \
  liquid-dsp/src/fec/src/fec_hamming74.c \
  liquid-dsp/src/fec/src/fec_hamming84.c \
  liquid-dsp/src/fec/src/fec_pass.c \
  liquid-dsp/src/fec/src/fec_rep3.c \
  liquid-dsp/src/fec/src/fec_rep5.c \
  liquid-dsp/src/fec/src/fec_rs.c \
  liquid-dsp/src/fec/src/fec_secded2216.c \
  liquid-dsp/src/fec/src/fec_secded3932.c \
  liquid-dsp/src/fec/src/fec_secded7264.c \
  liquid-dsp/src/fec/src/interleaver.c \
  liquid-dsp/src/fec/src/packetizer.c \
  liquid-dsp/src/fec/src/sumproduct.c \
  liquid-dsp/src/fft/src/fft_utilities.c \
  liquid-dsp/src/fft/src/fftf.c \
  liquid-dsp/src/fft/src/spgramf.c \
  liquid-dsp/src/filter/src/bessel.c \
  liquid-dsp/src/filter/src/butter.c \
  liquid-dsp/src/filter/src/cheby1.c \
  liquid-dsp/src/filter/src/cheby2.c \
  liquid-dsp/src/filter/src/ellip.c \
  liquid-dsp/src/filter/src/filter_cccf.c \
  liquid-dsp/src/filter/src/filter_crcf.c \
  liquid-dsp/src/filter/src/filter_rrrf.c \
  liquid-dsp/src/filter/src/firdes.c \
  liquid-dsp/src/filter/src/firdespm.c \
  liquid-dsp/src/filter/src/fnyquist.c \
  liquid-dsp/src/filter/src/gmsk.c \
  liquid-dsp/src/filter/src/group_delay.c \
  liquid-dsp/src/filter/src/hM3.c \
  liquid-dsp/src/filter/src/iirdes.c \
  liquid-dsp/src/filter/src/iirdes.pll.c \
  liquid-dsp/src/filter/src/lpc.c \
  liquid-dsp/src/filter/src/rcos.c \
  liquid-dsp/src/filter/src/rkaiser.c \
  liquid-dsp/src/filter/src/rrcos.c \
  liquid-dsp/src/framing/src/bpacketgen.c \
  liquid-dsp/src/framing/src/bpacketsync.c \
  liquid-dsp/src/framing/src/bpresync_cccf.c \
  liquid-dsp/src/framing/src/bsync_cccf.c \
  liquid-dsp/src/framing/src/bsync_crcf.c \
  liquid-dsp/src/framing/src/bsync_rrrf.c \
  liquid-dsp/src/framing/src/detector_cccf.c \
  liquid-dsp/src/framing/src/flexframegen.c \
  liquid-dsp/src/framing/src/flexframesync.c \
  liquid-dsp/src/framing/src/framedatastats.c \
  liquid-dsp/src/framing/src/framegen64.c \
  liquid-dsp/src/framing/src/framesync64.c \
  liquid-dsp/src/framing/src/framesyncstats.c \
  liquid-dsp/src/framing/src/gmskframegen.c \
  liquid-dsp/src/framing/src/gmskframesync.c \
  liquid-dsp/src/framing/src/ofdmflexframegen.c \
  liquid-dsp/src/framing/src/ofdmflexframesync.c \
  liquid-dsp/src/framing/src/presync_cccf.c \
  liquid-dsp/src/framing/src/qdetector_cccf.c \
  liquid-dsp/src/framing/src/qpacketmodem.c \
  liquid-dsp/src/framing/src/qpilotgen.c \
  liquid-dsp/src/framing/src/qpilotsync.c \
  liquid-dsp/src/math/src/math.bessel.c \
  liquid-dsp/src/math/src/math.c \
  liquid-dsp/src/math/src/math.complex.c \
  liquid-dsp/src/math/src/math.gamma.c \
  liquid-dsp/src/math/src/math.trig.c \
  liquid-dsp/src/math/src/modular_arithmetic.c \
  liquid-dsp/src/math/src/poly.c \
  liquid-dsp/src/math/src/polyc.c \
  liquid-dsp/src/math/src/polyc.fleetcents.c \
  liquid-dsp/src/math/src/polycf.c \
  liquid-dsp/src/math/src/polyf.c \
  liquid-dsp/src/matrix/src/matrix.c \
  liquid-dsp/src/matrix/src/matrixc.c \
  liquid-dsp/src/matrix/src/matrixcf.c \
  liquid-dsp/src/matrix/src/matrixf.c \
  liquid-dsp/src/matrix/src/smatrix.common.c \
  liquid-dsp/src/matrix/src/smatrixb.c \
  liquid-dsp/src/matrix/src/smatrixf.c \
  liquid-dsp/src/matrix/src/smatrixi.c \
  liquid-dsp/src/modem/src/ampmodem.c \
  liquid-dsp/src/modem/src/gmskdem.c \
  liquid-dsp/src/modem/src/gmskmod.c \
  liquid-dsp/src/modem/src/modem_apsk_const.c \
  liquid-dsp/src/modem/src/modem_arb_const.c \
  liquid-dsp/src/modem/src/modem_utilities.c \
  liquid-dsp/src/modem/src/modemf.c \
  liquid-dsp/src/multichannel/src/firpfbch_cccf.c \
  liquid-dsp/src/multichannel/src/firpfbch_crcf.c \
  liquid-dsp/src/multichannel/src/ofdmframe.common.c \
  liquid-dsp/src/multichannel/src/ofdmframegen.c \
  liquid-dsp/src/multichannel/src/ofdmframesync.c \
  liquid-dsp/src/nco/src/nco.utilities.c \
  liquid-dsp/src/nco/src/nco_crcf.c \
  liquid-dsp/src/optim/src/chromosome.c \
  liquid-dsp/src/optim/src/gasearch.c \
  liquid-dsp/src/optim/src/gradsearch.c \
  liquid-dsp/src/optim/src/optim.common.c \
  liquid-dsp/src/optim/src/qnsearch.c \
  liquid-dsp/src/optim/src/utilities.c \
  liquid-dsp/src/quantization/src/compand.c \
  liquid-dsp/src/quantization/src/quantizer.inline.c \
  liquid-dsp/src/quantization/src/quantizercf.c \
  liquid-dsp/src/quantization/src/quantizerf.c \
  liquid-dsp/src/random/src/rand.c \
  liquid-dsp/src/random/src/randexp.c \
  liquid-dsp/src/random/src/randgamma.c \
  liquid-dsp/src/random/src/randn.c \
  liquid-dsp/src/random/src/randnakm.c \
  liquid-dsp/src/random/src/randricek.c \
  liquid-dsp/src/random/src/randweib.c \
  liquid-dsp/src/random/src/scramble.c \
  liquid-dsp/src/sequence/src/bsequence.c \
  liquid-dsp/src/sequence/src/msequence.c \
  liquid-dsp/src/utility/src/bshift_array.c \
  liquid-dsp/src/utility/src/byte_utilities.c \
  liquid-dsp/src/utility/src/msb_index.c \
  liquid-dsp/src/utility/src/pack_bytes.c \
  liquid-dsp/src/utility/src/shift_array.c \
  liquid-dsp/src/vector/src/vectorcf_add.port.c \
  liquid-dsp/src/vector/src/vectorcf_mul.port.c \
  liquid-dsp/src/vector/src/vectorcf_norm.port.c \
  liquid-dsp/src/vector/src/vectorcf_trig.port.c \
  liquid-dsp/src/vector/src/vectorf_add.port.c \
  liquid-dsp/src/vector/src/vectorf_mul.port.c \
  liquid-dsp/src/vector/src/vectorf_norm.port.c \
  liquid-dsp/src/vector/src/vectorf_trig.port.c \
  liquid-dsp/src/libliquid.c
LOCAL_LDLIBS := -lm -llog
LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/liquid-dsp \
  $(LOCAL_PATH)/liquid-dsp/include \
  $(LOCAL_PATH)/fleetcents
LOCAL_CFLAGS := -Wall -fPIC -g -O2
LOCAL_MODULE := liquid
include $(BUILD_SHARED_LIBRARY)


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
  $(LOCAL_PATH)/fleetcents \
  $(LOCAL_PATH)/liquid-dsp/include
LOCAL_SHARED_LIBRARIES :=  liquid
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
