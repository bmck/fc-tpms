// auto-generated file, do not edit
// invoked with script type 'autotest' and delimiter '/'
// the following types need to be defined externally:
//      typedef ...(autotest_function_t)(...);
//      typedef struct {
//          unsigned int id;        // script identification number
//          autotest_function_t * api;    // pointer to function API
//          const char * name;      // script name
//          ...
//      } autotest_t;
//      typedef struct {
//          unsigned int id;        // package identification number
//          unsigned int index;     // index of first script
//          unsigned int n;         // number of scripts in package
//          const char * name;      // name of package
//      } package_t;

#ifndef __LIQUID_AUTOSCRIPT_INCLUDE_H__
#define __LIQUID_AUTOSCRIPT_INCLUDE_H__

#define AUTOSCRIPT_VERSION "0.3.1"

// number of packages
#define NUM_PACKAGES (95)

// number of scripts
#define NUM_AUTOSCRIPTS (610)

// function declarations
// autotest/null_autotest.c
autotest_function_t autotest_null;
// src/agc/tests/agc_crcf_autotest.c
autotest_function_t autotest_agc_crcf_dc_gain_control;
autotest_function_t autotest_agc_crcf_ac_gain_control;
autotest_function_t autotest_agc_crcf_rssi_sinusoid;
autotest_function_t autotest_agc_crcf_rssi_noise;
// src/audio/tests/cvsd_autotest.c
autotest_function_t autotest_cvsd_rmse_sine;
// src/buffer/tests/cbuffer_autotest.c
autotest_function_t autotest_cbufferf;
autotest_function_t autotest_cbuffercf;
autotest_function_t autotest_cbufferf_flow;
// src/buffer/tests/wdelay_autotest.c
autotest_function_t autotest_wdelayf;
// src/buffer/tests/window_autotest.c
autotest_function_t autotest_windowf;
// src/dotprod/tests/dotprod_rrrf_autotest.c
autotest_function_t autotest_dotprod_rrrf_basic;
autotest_function_t autotest_dotprod_rrrf_uneven;
autotest_function_t autotest_dotprod_rrrf_struct;
autotest_function_t autotest_dotprod_rrrf_struct_align;
autotest_function_t autotest_dotprod_rrrf_rand01;
autotest_function_t autotest_dotprod_rrrf_rand02;
autotest_function_t autotest_dotprod_rrrf_struct_lengths;
autotest_function_t autotest_dotprod_rrrf_struct_vs_ordinal;
// src/dotprod/tests/dotprod_crcf_autotest.c
autotest_function_t autotest_dotprod_crcf_rand01;
autotest_function_t autotest_dotprod_crcf_rand02;
autotest_function_t autotest_dotprod_crcf_struct_vs_ordinal;
// src/dotprod/tests/dotprod_cccf_autotest.c
autotest_function_t autotest_dotprod_cccf_rand16;
autotest_function_t autotest_dotprod_cccf_struct_lengths;
autotest_function_t autotest_dotprod_cccf_struct_vs_ordinal;
// src/dotprod/tests/sumsqf_autotest.c
autotest_function_t autotest_sumsqf_3;
autotest_function_t autotest_sumsqf_4;
autotest_function_t autotest_sumsqf_7;
autotest_function_t autotest_sumsqf_8;
autotest_function_t autotest_sumsqf_15;
autotest_function_t autotest_sumsqf_16;
// src/dotprod/tests/sumsqcf_autotest.c
autotest_function_t autotest_sumsqcf_3;
autotest_function_t autotest_sumsqcf_4;
autotest_function_t autotest_sumsqcf_7;
autotest_function_t autotest_sumsqcf_8;
autotest_function_t autotest_sumsqcf_15;
autotest_function_t autotest_sumsqcf_16;
// src/equalization/tests/eqrls_rrrf_autotest.c
autotest_function_t autotest_eqrls_rrrf_01;
// src/fec/tests/crc_autotest.c
autotest_function_t autotest_reverse_byte;
autotest_function_t autotest_reverse_uint16;
autotest_function_t autotest_reverse_uint32;
autotest_function_t autotest_checksum;
autotest_function_t autotest_crc8;
autotest_function_t autotest_crc16;
autotest_function_t autotest_crc24;
autotest_function_t autotest_crc32;
// src/fec/tests/fec_autotest.c
autotest_function_t autotest_fec_r3;
autotest_function_t autotest_fec_r5;
autotest_function_t autotest_fec_h74;
autotest_function_t autotest_fec_h84;
autotest_function_t autotest_fec_h128;
autotest_function_t autotest_fec_g2412;
autotest_function_t autotest_fec_secded2216;
autotest_function_t autotest_fec_secded3932;
autotest_function_t autotest_fec_secded7264;
autotest_function_t autotest_fec_v27;
autotest_function_t autotest_fec_v29;
autotest_function_t autotest_fec_v39;
autotest_function_t autotest_fec_v615;
autotest_function_t autotest_fec_v27p23;
autotest_function_t autotest_fec_v27p34;
autotest_function_t autotest_fec_v27p45;
autotest_function_t autotest_fec_v27p56;
autotest_function_t autotest_fec_v27p67;
autotest_function_t autotest_fec_v27p78;
autotest_function_t autotest_fec_v29p23;
autotest_function_t autotest_fec_v29p34;
autotest_function_t autotest_fec_v29p45;
autotest_function_t autotest_fec_v29p56;
autotest_function_t autotest_fec_v29p67;
autotest_function_t autotest_fec_v29p78;
autotest_function_t autotest_fec_rs8;
// src/fec/tests/fec_soft_autotest.c
autotest_function_t autotest_fecsoft_r3;
autotest_function_t autotest_fecsoft_r5;
autotest_function_t autotest_fecsoft_h74;
autotest_function_t autotest_fecsoft_h84;
autotest_function_t autotest_fecsoft_h128;
autotest_function_t autotest_fecsoft_v27;
autotest_function_t autotest_fecsoft_v29;
autotest_function_t autotest_fecsoft_v39;
autotest_function_t autotest_fecsoft_v615;
autotest_function_t autotest_fecsoft_v27p23;
autotest_function_t autotest_fecsoft_v27p34;
autotest_function_t autotest_fecsoft_v27p45;
autotest_function_t autotest_fecsoft_v27p56;
autotest_function_t autotest_fecsoft_v27p67;
autotest_function_t autotest_fecsoft_v27p78;
autotest_function_t autotest_fecsoft_v29p23;
autotest_function_t autotest_fecsoft_v29p34;
autotest_function_t autotest_fecsoft_v29p45;
autotest_function_t autotest_fecsoft_v29p56;
autotest_function_t autotest_fecsoft_v29p67;
autotest_function_t autotest_fecsoft_v29p78;
autotest_function_t autotest_fecsoft_rs8;
// src/fec/tests/fec_golay2412_autotest.c
autotest_function_t autotest_golay2412_codec;
// src/fec/tests/fec_hamming74_autotest.c
autotest_function_t autotest_hamming74_codec;
autotest_function_t autotest_hamming74_codec_soft;
// src/fec/tests/fec_hamming84_autotest.c
autotest_function_t autotest_hamming84_codec;
autotest_function_t autotest_hamming84_codec_soft;
// src/fec/tests/fec_hamming128_autotest.c
autotest_function_t autotest_hamming128_codec;
autotest_function_t autotest_hamming128_codec_soft;
// src/fec/tests/fec_hamming1511_autotest.c
autotest_function_t autotest_hamming1511_codec;
// src/fec/tests/fec_hamming3126_autotest.c
autotest_function_t autotest_hamming3126_codec;
// src/fec/tests/fec_reedsolomon_autotest.c
autotest_function_t autotest_reedsolomon_223_255;
// src/fec/tests/fec_rep3_autotest.c
autotest_function_t autotest_rep3_codec;
// src/fec/tests/fec_rep5_autotest.c
autotest_function_t autotest_rep5_codec;
// src/fec/tests/fec_secded2216_autotest.c
autotest_function_t autotest_secded2216_codec_e0;
autotest_function_t autotest_secded2216_codec_e1;
autotest_function_t autotest_secded2216_codec_e2;
// src/fec/tests/fec_secded3932_autotest.c
autotest_function_t autotest_secded3932_codec_e0;
autotest_function_t autotest_secded3932_codec_e1;
autotest_function_t autotest_secded3932_codec_e2;
// src/fec/tests/fec_secded7264_autotest.c
autotest_function_t autotest_secded7264_codec_e0;
autotest_function_t autotest_secded7264_codec_e1;
autotest_function_t autotest_secded7264_codec_e2;
// src/fec/tests/interleaver_autotest.c
autotest_function_t autotest_interleaver_hard_8;
autotest_function_t autotest_interleaver_hard_16;
autotest_function_t autotest_interleaver_hard_64;
autotest_function_t autotest_interleaver_hard_256;
autotest_function_t autotest_interleaver_soft_8;
autotest_function_t autotest_interleaver_soft_16;
autotest_function_t autotest_interleaver_soft_64;
autotest_function_t autotest_interleaver_soft_256;
// src/fec/tests/packetizer_autotest.c
autotest_function_t autotest_packetizer_n16_0_0;
autotest_function_t autotest_packetizer_n16_0_1;
autotest_function_t autotest_packetizer_n16_0_2;
// src/fft/tests/fft_small_autotest.c
autotest_function_t autotest_fft_3;
autotest_function_t autotest_fft_5;
autotest_function_t autotest_fft_6;
autotest_function_t autotest_fft_7;
autotest_function_t autotest_fft_9;
// src/fft/tests/fft_radix2_autotest.c
autotest_function_t autotest_fft_2;
autotest_function_t autotest_fft_4;
autotest_function_t autotest_fft_8;
autotest_function_t autotest_fft_16;
autotest_function_t autotest_fft_32;
autotest_function_t autotest_fft_64;
// src/fft/tests/fft_composite_autotest.c
autotest_function_t autotest_fft_10;
autotest_function_t autotest_fft_21;
autotest_function_t autotest_fft_22;
autotest_function_t autotest_fft_24;
autotest_function_t autotest_fft_26;
autotest_function_t autotest_fft_30;
autotest_function_t autotest_fft_35;
autotest_function_t autotest_fft_36;
autotest_function_t autotest_fft_48;
autotest_function_t autotest_fft_63;
autotest_function_t autotest_fft_92;
autotest_function_t autotest_fft_96;
autotest_function_t autotest_fft_120;
autotest_function_t autotest_fft_130;
autotest_function_t autotest_fft_192;
// src/fft/tests/fft_prime_autotest.c
autotest_function_t autotest_fft_17;
autotest_function_t autotest_fft_43;
autotest_function_t autotest_fft_79;
autotest_function_t autotest_fft_157;
autotest_function_t autotest_fft_317;
autotest_function_t autotest_fft_509;
// src/fft/tests/fft_r2r_autotest.c
autotest_function_t autotest_fft_r2r_REDFT00_n8;
autotest_function_t autotest_fft_r2r_REDFT10_n8;
autotest_function_t autotest_fft_r2r_REDFT01_n8;
autotest_function_t autotest_fft_r2r_REDFT11_n8;
autotest_function_t autotest_fft_r2r_RODFT00_n8;
autotest_function_t autotest_fft_r2r_RODFT10_n8;
autotest_function_t autotest_fft_r2r_RODFT01_n8;
autotest_function_t autotest_fft_r2r_RODFT11_n8;
autotest_function_t autotest_fft_r2r_REDFT00_n32;
autotest_function_t autotest_fft_r2r_REDFT10_n32;
autotest_function_t autotest_fft_r2r_REDFT01_n32;
autotest_function_t autotest_fft_r2r_REDFT11_n32;
autotest_function_t autotest_fft_r2r_RODFT00_n32;
autotest_function_t autotest_fft_r2r_RODFT10_n32;
autotest_function_t autotest_fft_r2r_RODFT01_n32;
autotest_function_t autotest_fft_r2r_RODFT11_n32;
autotest_function_t autotest_fft_r2r_REDFT00_n27;
autotest_function_t autotest_fft_r2r_REDFT10_n27;
autotest_function_t autotest_fft_r2r_REDFT01_n27;
autotest_function_t autotest_fft_r2r_REDFT11_n27;
autotest_function_t autotest_fft_r2r_RODFT00_n27;
autotest_function_t autotest_fft_r2r_RODFT10_n27;
autotest_function_t autotest_fft_r2r_RODFT01_n27;
autotest_function_t autotest_fft_r2r_RODFT11_n27;
// src/fft/tests/fft_shift_autotest.c
autotest_function_t autotest_fft_shift_4;
autotest_function_t autotest_fft_shift_8;
// src/filter/tests/fftfilt_xxxf_autotest.c
autotest_function_t autotest_fftfilt_rrrf_data_h4x256;
autotest_function_t autotest_fftfilt_rrrf_data_h7x256;
autotest_function_t autotest_fftfilt_rrrf_data_h13x256;
autotest_function_t autotest_fftfilt_rrrf_data_h23x256;
autotest_function_t autotest_fftfilt_crcf_data_h4x256;
autotest_function_t autotest_fftfilt_crcf_data_h7x256;
autotest_function_t autotest_fftfilt_crcf_data_h13x256;
autotest_function_t autotest_fftfilt_crcf_data_h23x256;
autotest_function_t autotest_fftfilt_cccf_data_h4x256;
autotest_function_t autotest_fftfilt_cccf_data_h7x256;
autotest_function_t autotest_fftfilt_cccf_data_h13x256;
autotest_function_t autotest_fftfilt_cccf_data_h23x256;
// src/filter/tests/filter_crosscorr_autotest.c
autotest_function_t autotest_filter_crosscorr_rrrf;
// src/filter/tests/firdecim_xxxf_autotest.c
autotest_function_t autotest_firdecim_rrrf_data_M2h4x20;
autotest_function_t autotest_firdecim_rrrf_data_M3h7x30;
autotest_function_t autotest_firdecim_rrrf_data_M4h13x40;
autotest_function_t autotest_firdecim_rrrf_data_M5h23x50;
autotest_function_t autotest_firdecim_crcf_data_M2h4x20;
autotest_function_t autotest_firdecim_crcf_data_M3h7x30;
autotest_function_t autotest_firdecim_crcf_data_M4h13x40;
autotest_function_t autotest_firdecim_crcf_data_M5h23x50;
autotest_function_t autotest_firdecim_cccf_data_M2h4x20;
autotest_function_t autotest_firdecim_cccf_data_M3h7x30;
autotest_function_t autotest_firdecim_cccf_data_M4h13x40;
autotest_function_t autotest_firdecim_cccf_data_M5h23x50;
// src/filter/tests/firdes_autotest.c
autotest_function_t autotest_liquid_firdes_rcos;
autotest_function_t autotest_liquid_firdes_rrcos;
autotest_function_t autotest_liquid_firdes_rkaiser;
// src/filter/tests/firdespm_autotest.c
autotest_function_t autotest_firdespm_bandpass_n24;
autotest_function_t autotest_firdespm_bandpass_n32;
// src/filter/tests/firfilt_xxxf_autotest.c
autotest_function_t autotest_firfilt_rrrf_data_h4x8;
autotest_function_t autotest_firfilt_rrrf_data_h7x16;
autotest_function_t autotest_firfilt_rrrf_data_h13x32;
autotest_function_t autotest_firfilt_rrrf_data_h23x64;
autotest_function_t autotest_firfilt_crcf_data_h4x8;
autotest_function_t autotest_firfilt_crcf_data_h7x16;
autotest_function_t autotest_firfilt_crcf_data_h13x32;
autotest_function_t autotest_firfilt_crcf_data_h23x64;
autotest_function_t autotest_firfilt_cccf_data_h4x8;
autotest_function_t autotest_firfilt_cccf_data_h7x16;
autotest_function_t autotest_firfilt_cccf_data_h13x32;
autotest_function_t autotest_firfilt_cccf_data_h23x64;
// src/filter/tests/firhilb_autotest.c
autotest_function_t autotest_firhilbf_decim;
autotest_function_t autotest_firhilbf_interp;
// src/filter/tests/firinterp_autotest.c
autotest_function_t autotest_firinterp_rrrf_generic;
autotest_function_t autotest_firinterp_crcf_generic;
// src/filter/tests/firpfb_autotest.c
autotest_function_t autotest_firpfb_impulse_response;
// src/filter/tests/groupdelay_autotest.c
autotest_function_t autotest_fir_groupdelay_n3;
autotest_function_t autotest_iir_groupdelay_n3;
autotest_function_t autotest_iir_groupdelay_n8;
autotest_function_t autotest_iir_groupdelay_sos_n8;
// src/filter/tests/iirdes_autotest.c
autotest_function_t autotest_iirdes_butter_2;
autotest_function_t autotest_iirdes_cplxpair_n6;
autotest_function_t autotest_iirdes_cplxpair_n20;
autotest_function_t autotest_iirdes_dzpk2sosf;
autotest_function_t autotest_iirdes_isstable_n2_yes;
autotest_function_t autotest_iirdes_isstable_n2_no;
// src/filter/tests/iirfilt_xxxf_autotest.c
autotest_function_t autotest_iirfilt_rrrf_h3x64;
autotest_function_t autotest_iirfilt_rrrf_h5x64;
autotest_function_t autotest_iirfilt_rrrf_h7x64;
autotest_function_t autotest_iirfilt_crcf_h3x64;
autotest_function_t autotest_iirfilt_crcf_h5x64;
autotest_function_t autotest_iirfilt_crcf_h7x64;
autotest_function_t autotest_iirfilt_cccf_h3x64;
autotest_function_t autotest_iirfilt_cccf_h5x64;
autotest_function_t autotest_iirfilt_cccf_h7x64;
// src/filter/tests/iirfiltsos_rrrf_autotest.c
autotest_function_t autotest_iirfiltsos_impulse_n2;
autotest_function_t autotest_iirfiltsos_step_n2;
// src/filter/tests/msresamp_crcf_autotest.c
autotest_function_t autotest_msresamp_crcf;
// src/filter/tests/resamp_crcf_autotest.c
autotest_function_t autotest_resamp_crcf;
// src/filter/tests/resamp2_crcf_autotest.c
autotest_function_t autotest_resamp2_analysis;
autotest_function_t autotest_resamp2_synthesis;
// src/filter/tests/symsync_crcf_autotest.c
autotest_function_t autotest_symsync_crcf_scenario_0;
autotest_function_t autotest_symsync_crcf_scenario_1;
autotest_function_t autotest_symsync_crcf_scenario_2;
autotest_function_t autotest_symsync_crcf_scenario_3;
// src/filter/tests/symsync_rrrf_autotest.c
autotest_function_t autotest_symsync_rrrf_scenario_0;
autotest_function_t autotest_symsync_rrrf_scenario_1;
autotest_function_t autotest_symsync_rrrf_scenario_2;
autotest_function_t autotest_symsync_rrrf_scenario_3;
// src/framing/tests/bpacketsync_autotest.c
autotest_function_t autotest_bpacketsync;
// src/framing/tests/bsync_autotest.c
autotest_function_t autotest_bsync_rrrf_15;
autotest_function_t autotest_bsync_crcf_15;
// src/framing/tests/detector_autotest.c
autotest_function_t autotest_detector_cccf_n64;
autotest_function_t autotest_detector_cccf_n83;
autotest_function_t autotest_detector_cccf_n128;
autotest_function_t autotest_detector_cccf_n167;
autotest_function_t autotest_detector_cccf_n256;
autotest_function_t autotest_detector_cccf_n335;
autotest_function_t autotest_detector_cccf_n512;
autotest_function_t autotest_detector_cccf_n671;
autotest_function_t autotest_detector_cccf_n1024;
autotest_function_t autotest_detector_cccf_n1341;
// src/framing/tests/flexframesync_autotest.c
autotest_function_t autotest_flexframesync;
// src/framing/tests/framesync64_autotest.c
autotest_function_t autotest_framesync64;
// src/framing/tests/qdetector_cccf_autotest.c
autotest_function_t autotest_qdetector_cccf_linear_n64;
autotest_function_t autotest_qdetector_cccf_linear_n83;
autotest_function_t autotest_qdetector_cccf_linear_n128;
autotest_function_t autotest_qdetector_cccf_linear_n167;
autotest_function_t autotest_qdetector_cccf_linear_n256;
autotest_function_t autotest_qdetector_cccf_linear_n335;
autotest_function_t autotest_qdetector_cccf_linear_n512;
autotest_function_t autotest_qdetector_cccf_linear_n671;
autotest_function_t autotest_qdetector_cccf_linear_n1024;
autotest_function_t autotest_qdetector_cccf_linear_n1341;
autotest_function_t autotest_qdetector_cccf_gmsk_n64;
autotest_function_t autotest_qdetector_cccf_gmsk_n83;
autotest_function_t autotest_qdetector_cccf_gmsk_n128;
autotest_function_t autotest_qdetector_cccf_gmsk_n167;
autotest_function_t autotest_qdetector_cccf_gmsk_n256;
autotest_function_t autotest_qdetector_cccf_gmsk_n335;
autotest_function_t autotest_qdetector_cccf_gmsk_n512;
autotest_function_t autotest_qdetector_cccf_gmsk_n671;
autotest_function_t autotest_qdetector_cccf_gmsk_n1024;
autotest_function_t autotest_qdetector_cccf_gmsk_n1341;
// src/framing/tests/qpacketmodem_autotest.c
autotest_function_t autotest_qpacketmodem_bpsk;
autotest_function_t autotest_qpacketmodem_qpsk;
autotest_function_t autotest_qpacketmodem_psk8;
autotest_function_t autotest_qpacketmodem_qam16;
autotest_function_t autotest_qpacketmodem_sqam32;
autotest_function_t autotest_qpacketmodem_qam64;
autotest_function_t autotest_qpacketmodem_sqam128;
autotest_function_t autotest_qpacketmodem_qam256;
// src/framing/tests/qpilotsync_autotest.c
autotest_function_t autotest_qpilotsync_100_16;
autotest_function_t autotest_qpilotsync_200_20;
autotest_function_t autotest_qpilotsync_300_24;
autotest_function_t autotest_qpilotsync_400_28;
autotest_function_t autotest_qpilotsync_500_32;
// src/math/tests/kbd_autotest.c
autotest_function_t autotest_kbd_n16;
autotest_function_t autotest_kbd_n32;
autotest_function_t autotest_kbd_n48;
// src/math/tests/math_autotest.c
autotest_function_t autotest_Q;
autotest_function_t autotest_sincf;
autotest_function_t autotest_nextpow2;
// src/math/tests/math_bessel_autotest.c
autotest_function_t autotest_lnbesselif;
autotest_function_t autotest_besselif;
autotest_function_t autotest_besseli0f;
autotest_function_t autotest_besseljf;
autotest_function_t autotest_besselj0f;
// src/math/tests/math_gamma_autotest.c
autotest_function_t autotest_gamma;
autotest_function_t autotest_lngamma;
autotest_function_t autotest_uppergamma;
autotest_function_t autotest_factorial;
autotest_function_t autotest_nchoosek;
// src/math/tests/math_complex_autotest.c
autotest_function_t autotest_cexpf;
autotest_function_t autotest_clogf;
autotest_function_t autotest_csqrtf;
autotest_function_t autotest_casinf;
autotest_function_t autotest_cacosf;
autotest_function_t autotest_catanf;
// src/math/tests/polynomial_autotest.c
autotest_function_t autotest_polyf_fit_q3n3;
autotest_function_t autotest_polyf_expandroots_4;
autotest_function_t autotest_polyf_expandroots_11;
autotest_function_t autotest_polycf_expandroots_4;
autotest_function_t autotest_polyf_expandroots2_3;
autotest_function_t autotest_polyf_mul_2_3;
autotest_function_t autotest_poly_expandbinomial_n6;
autotest_function_t autotest_poly_binomial_expand_pm_m6_k1;
autotest_function_t autotest_poly_expandbinomial_pm_m5_k2;
autotest_function_t autotest_polyf_findroots;
// src/matrix/tests/matrixcf_autotest.c
autotest_function_t autotest_matrixcf_add;
autotest_function_t autotest_matrixcf_aug;
autotest_function_t autotest_matrixcf_chol;
autotest_function_t autotest_matrixcf_inv;
autotest_function_t autotest_matrixcf_linsolve;
autotest_function_t autotest_matrixcf_ludecomp_crout;
autotest_function_t autotest_matrixcf_ludecomp_doolittle;
autotest_function_t autotest_matrixcf_mul;
autotest_function_t autotest_matrixcf_qrdecomp;
autotest_function_t autotest_matrixcf_transmul;
// src/matrix/tests/matrixf_autotest.c
autotest_function_t autotest_matrixf_add;
autotest_function_t autotest_matrixf_aug;
autotest_function_t autotest_matrixf_cgsolve;
autotest_function_t autotest_matrixf_chol;
autotest_function_t autotest_matrixf_gramschmidt;
autotest_function_t autotest_matrixf_inv;
autotest_function_t autotest_matrixf_linsolve;
autotest_function_t autotest_matrixf_ludecomp_crout;
autotest_function_t autotest_matrixf_ludecomp_doolittle;
autotest_function_t autotest_matrixf_mul;
autotest_function_t autotest_matrixf_qrdecomp;
autotest_function_t autotest_matrixf_transmul;
// src/matrix/tests/smatrixb_autotest.c
autotest_function_t autotest_smatrixb_vmul;
autotest_function_t autotest_smatrixb_mul;
autotest_function_t autotest_smatrixb_mulf;
autotest_function_t autotest_smatrixb_vmulf;
// src/matrix/tests/smatrixf_autotest.c
autotest_function_t autotest_smatrixf_vmul;
autotest_function_t autotest_smatrixf_mul;
// src/matrix/tests/smatrixi_autotest.c
autotest_function_t autotest_smatrixi_vmul;
autotest_function_t autotest_smatrixi_mul;
// src/modem/tests/freqmodem_autotest.c
autotest_function_t autotest_freqmodem_kf_0_02;
autotest_function_t autotest_freqmodem_kf_0_04;
autotest_function_t autotest_freqmodem_kf_0_08;
// src/modem/tests/modem_autotest.c
autotest_function_t autotest_mod_demod_psk2;
autotest_function_t autotest_mod_demod_psk4;
autotest_function_t autotest_mod_demod_psk8;
autotest_function_t autotest_mod_demod_psk16;
autotest_function_t autotest_mod_demod_psk32;
autotest_function_t autotest_mod_demod_psk64;
autotest_function_t autotest_mod_demod_psk128;
autotest_function_t autotest_mod_demod_psk256;
autotest_function_t autotest_mod_demod_dpsk2;
autotest_function_t autotest_mod_demod_dpsk4;
autotest_function_t autotest_mod_demod_dpsk8;
autotest_function_t autotest_mod_demod_dpsk16;
autotest_function_t autotest_mod_demod_dpsk32;
autotest_function_t autotest_mod_demod_dpsk64;
autotest_function_t autotest_mod_demod_dpsk128;
autotest_function_t autotest_mod_demod_dpsk256;
autotest_function_t autotest_mod_demod_ask2;
autotest_function_t autotest_mod_demod_ask4;
autotest_function_t autotest_mod_demod_ask8;
autotest_function_t autotest_mod_demod_ask16;
autotest_function_t autotest_mod_demod_ask32;
autotest_function_t autotest_mod_demod_ask64;
autotest_function_t autotest_mod_demod_ask128;
autotest_function_t autotest_mod_demod_ask256;
autotest_function_t autotest_mod_demod_qam4;
autotest_function_t autotest_mod_demod_qam8;
autotest_function_t autotest_mod_demod_qam16;
autotest_function_t autotest_mod_demod_qam32;
autotest_function_t autotest_mod_demod_qam64;
autotest_function_t autotest_mod_demod_qam128;
autotest_function_t autotest_mod_demod_qam256;
autotest_function_t autotest_mod_demod_apsk4;
autotest_function_t autotest_mod_demod_apsk8;
autotest_function_t autotest_mod_demod_apsk16;
autotest_function_t autotest_mod_demod_apsk32;
autotest_function_t autotest_mod_demod_apsk64;
autotest_function_t autotest_mod_demod_apsk128;
autotest_function_t autotest_mod_demod_apsk256;
autotest_function_t autotest_mod_demod_bpsk;
autotest_function_t autotest_mod_demod_qpsk;
autotest_function_t autotest_mod_demod_ook;
autotest_function_t autotest_mod_demod_sqam32;
autotest_function_t autotest_mod_demod_sqam128;
autotest_function_t autotest_mod_demod_V29;
autotest_function_t autotest_mod_demod_arb16opt;
autotest_function_t autotest_mod_demod_arb32opt;
autotest_function_t autotest_mod_demod_arb64opt;
autotest_function_t autotest_mod_demod_arb128opt;
autotest_function_t autotest_mod_demod_arb256opt;
autotest_function_t autotest_mod_demod_arb64vt;
// src/modem/tests/modem_demodsoft_autotest.c
autotest_function_t autotest_demodsoft_psk2;
autotest_function_t autotest_demodsoft_psk4;
autotest_function_t autotest_demodsoft_psk8;
autotest_function_t autotest_demodsoft_psk16;
autotest_function_t autotest_demodsoft_psk32;
autotest_function_t autotest_demodsoft_psk64;
autotest_function_t autotest_demodsoft_psk128;
autotest_function_t autotest_demodsoft_psk256;
autotest_function_t autotest_demodsoft_dpsk2;
autotest_function_t autotest_demodsoft_dpsk4;
autotest_function_t autotest_demodsoft_dpsk8;
autotest_function_t autotest_demodsoft_dpsk16;
autotest_function_t autotest_demodsoft_dpsk32;
autotest_function_t autotest_demodsoft_dpsk64;
autotest_function_t autotest_demodsoft_dpsk128;
autotest_function_t autotest_demodsoft_dpsk256;
autotest_function_t autotest_demodsoft_ask2;
autotest_function_t autotest_demodsoft_ask4;
autotest_function_t autotest_demodsoft_ask8;
autotest_function_t autotest_demodsoft_ask16;
autotest_function_t autotest_demodsoft_ask32;
autotest_function_t autotest_demodsoft_ask64;
autotest_function_t autotest_demodsoft_ask128;
autotest_function_t autotest_demodsoft_ask256;
autotest_function_t autotest_demodsoft_qam4;
autotest_function_t autotest_demodsoft_qam8;
autotest_function_t autotest_demodsoft_qam16;
autotest_function_t autotest_demodsoft_qam32;
autotest_function_t autotest_demodsoft_qam64;
autotest_function_t autotest_demodsoft_qam128;
autotest_function_t autotest_demodsoft_qam256;
autotest_function_t autotest_demodsoft_apsk4;
autotest_function_t autotest_demodsoft_apsk8;
autotest_function_t autotest_demodsoft_apsk16;
autotest_function_t autotest_demodsoft_apsk32;
autotest_function_t autotest_demodsoft_apsk64;
autotest_function_t autotest_demodsoft_apsk128;
autotest_function_t autotest_demodsoft_apsk256;
autotest_function_t autotest_demodsoft_bpsk;
autotest_function_t autotest_demodsoft_qpsk;
autotest_function_t autotest_demodsoft_ook;
autotest_function_t autotest_demodsoft_sqam32;
autotest_function_t autotest_demodsoft_sqam128;
autotest_function_t autotest_demodsoft_V29;
autotest_function_t autotest_demodsoft_arb16opt;
autotest_function_t autotest_demodsoft_arb32opt;
autotest_function_t autotest_demodsoft_arb64opt;
autotest_function_t autotest_demodsoft_arb128opt;
autotest_function_t autotest_demodsoft_arb256opt;
autotest_function_t autotest_demodsoft_arb64vt;
// src/modem/tests/modem_demodstats_autotest.c
autotest_function_t autotest_demodstats_psk2;
autotest_function_t autotest_demodstats_psk4;
autotest_function_t autotest_demodstats_psk8;
autotest_function_t autotest_demodstats_psk16;
autotest_function_t autotest_demodstats_psk32;
autotest_function_t autotest_demodstats_psk64;
autotest_function_t autotest_demodstats_psk128;
autotest_function_t autotest_demodstats_psk256;
autotest_function_t autotest_demodstats_dpsk2;
autotest_function_t autotest_demodstats_dpsk4;
autotest_function_t autotest_demodstats_dpsk8;
autotest_function_t autotest_demodstats_dpsk16;
autotest_function_t autotest_demodstats_dpsk32;
autotest_function_t autotest_demodstats_dpsk64;
autotest_function_t autotest_demodstats_dpsk128;
autotest_function_t autotest_demodstats_dpsk256;
autotest_function_t autotest_demodstats_ask2;
autotest_function_t autotest_demodstats_ask4;
autotest_function_t autotest_demodstats_ask8;
autotest_function_t autotest_demodstats_ask16;
autotest_function_t autotest_demodstats_ask32;
autotest_function_t autotest_demodstats_ask64;
autotest_function_t autotest_demodstats_ask128;
autotest_function_t autotest_demodstats_ask256;
autotest_function_t autotest_demodstats_qam4;
autotest_function_t autotest_demodstats_qam8;
autotest_function_t autotest_demodstats_qam16;
autotest_function_t autotest_demodstats_qam32;
autotest_function_t autotest_demodstats_qam64;
autotest_function_t autotest_demodstats_qam128;
autotest_function_t autotest_demodstats_qam256;
autotest_function_t autotest_demodstats_apsk4;
autotest_function_t autotest_demodstats_apsk8;
autotest_function_t autotest_demodstats_apsk16;
autotest_function_t autotest_demodstats_apsk32;
autotest_function_t autotest_demodstats_apsk64;
autotest_function_t autotest_demodstats_apsk128;
autotest_function_t autotest_demodstats_apsk256;
autotest_function_t autotest_demodstats_bpsk;
autotest_function_t autotest_demodstats_qpsk;
autotest_function_t autotest_demodstats_ook;
autotest_function_t autotest_demodstats_sqam32;
autotest_function_t autotest_demodstats_sqam128;
autotest_function_t autotest_demodstats_V29;
autotest_function_t autotest_demodstats_arb16opt;
autotest_function_t autotest_demodstats_arb32opt;
autotest_function_t autotest_demodstats_arb64opt;
autotest_function_t autotest_demodstats_arb128opt;
autotest_function_t autotest_demodstats_arb256opt;
autotest_function_t autotest_demodstats_arb64vt;
// src/multichannel/tests/firpfbch2_crcf_autotest.c
autotest_function_t autotest_firpfbch2_crcf_n8;
autotest_function_t autotest_firpfbch2_crcf_n16;
autotest_function_t autotest_firpfbch2_crcf_n32;
autotest_function_t autotest_firpfbch2_crcf_n64;
// src/multichannel/tests/firpfbch_crcf_synthesizer_autotest.c
autotest_function_t autotest_firpfbch_crcf_synthesis;
// src/multichannel/tests/firpfbch_crcf_analyzer_autotest.c
autotest_function_t autotest_firpfbch_crcf_analysis;
// src/multichannel/tests/ofdmframesync_autotest.c
autotest_function_t autotest_ofdmframesync_acquire_n64;
autotest_function_t autotest_ofdmframesync_acquire_n128;
autotest_function_t autotest_ofdmframesync_acquire_n256;
autotest_function_t autotest_ofdmframesync_acquire_n512;
// src/nco/tests/nco_crcf_frequency_autotest.c
autotest_function_t autotest_nco_crcf_frequency;
autotest_function_t autotest_vco_crcf_frequency;
// src/nco/tests/nco_crcf_phase_autotest.c
autotest_function_t autotest_nco_crcf_phase;
autotest_function_t autotest_vco_crcf_phase;
autotest_function_t autotest_nco_basic;
autotest_function_t autotest_nco_mixing;
autotest_function_t autotest_nco_block_mixing;
// src/nco/tests/nco_crcf_pll_autotest.c
autotest_function_t autotest_vco_crcf_pll_phase;
autotest_function_t autotest_nco_crcf_pll_phase;
// src/nco/tests/unwrap_phase_autotest.c
autotest_function_t autotest_nco_unwrap_phase;
// src/optim/tests/gradsearch_autotest.c
autotest_function_t autotest_gradsearch_rosenbrock;
autotest_function_t autotest_gradsearch_maxutility;
// src/quantization/tests/compand_autotest.c
autotest_function_t autotest_compand_float;
autotest_function_t autotest_compand_cfloat;
// src/quantization/tests/quantize_autotest.c
autotest_function_t autotest_quantize_float_n8;
// src/random/tests/scramble_autotest.c
autotest_function_t autotest_scramble_n16;
autotest_function_t autotest_scramble_n64;
autotest_function_t autotest_scramble_n256;
autotest_function_t autotest_scramble_n11;
autotest_function_t autotest_scramble_n33;
autotest_function_t autotest_scramble_n277;
autotest_function_t autotest_scramble_soft_n16;
autotest_function_t autotest_scramble_soft_n64;
autotest_function_t autotest_scramble_soft_n256;
autotest_function_t autotest_scramble_soft_n11;
autotest_function_t autotest_scramble_soft_n33;
autotest_function_t autotest_scramble_soft_n277;
// src/sequence/tests/bsequence_autotest.c
autotest_function_t autotest_bsequence_init;
autotest_function_t autotest_bsequence_correlate;
autotest_function_t autotest_bsequence_add;
autotest_function_t autotest_bsequence_mul;
autotest_function_t autotest_bsequence_accumulate;
// src/sequence/tests/complementary_codes_autotest.c
autotest_function_t autotest_complementary_code_n8;
autotest_function_t autotest_complementary_code_n16;
autotest_function_t autotest_complementary_code_n32;
autotest_function_t autotest_complementary_code_n64;
autotest_function_t autotest_complementary_code_n128;
autotest_function_t autotest_complementary_code_n256;
autotest_function_t autotest_complementary_code_n512;
// src/sequence/tests/msequence_autotest.c
autotest_function_t autotest_bsequence_init_msequence;
autotest_function_t autotest_msequence_m2;
autotest_function_t autotest_msequence_m3;
autotest_function_t autotest_msequence_m4;
autotest_function_t autotest_msequence_m5;
autotest_function_t autotest_msequence_m6;
autotest_function_t autotest_msequence_m7;
autotest_function_t autotest_msequence_m8;
autotest_function_t autotest_msequence_m9;
autotest_function_t autotest_msequence_m10;
autotest_function_t autotest_msequence_m11;
autotest_function_t autotest_msequence_m12;
// src/utility/tests/bshift_array_autotest.c
autotest_function_t autotest_lbshift;
autotest_function_t autotest_rbshift;
autotest_function_t autotest_lbcircshift;
autotest_function_t autotest_rbcircshift;
// src/utility/tests/count_bits_autotest.c
autotest_function_t autotest_count_ones;
autotest_function_t autotest_count_ones_mod2;
autotest_function_t autotest_bdotprod;
autotest_function_t autotest_count_leading_zeros;
autotest_function_t autotest_msb_index;
// src/utility/tests/pack_bytes_autotest.c
autotest_function_t autotest_pack_array;
autotest_function_t autotest_unpack_array;
autotest_function_t autotest_repack_array;
autotest_function_t autotest_pack_bytes_01;
autotest_function_t autotest_unpack_bytes_01;
autotest_function_t autotest_repack_bytes_01;
autotest_function_t autotest_repack_bytes_02;
autotest_function_t autotest_repack_bytes_03;
autotest_function_t autotest_repack_bytes_04_uneven;
// src/utility/tests/shift_array_autotest.c
autotest_function_t autotest_lshift;
autotest_function_t autotest_rshift;
autotest_function_t autotest_lcircshift;
autotest_function_t autotest_rcircshift;

// array of scripts
autotest_t scripts[NUM_AUTOSCRIPTS] = {
    {.id =    0, .api = &autotest_null,                           .name = "null"},
    {.id =    1, .api = &autotest_agc_crcf_dc_gain_control,       .name = "agc_crcf_dc_gain_control"},
    {.id =    2, .api = &autotest_agc_crcf_ac_gain_control,       .name = "agc_crcf_ac_gain_control"},
    {.id =    3, .api = &autotest_agc_crcf_rssi_sinusoid,         .name = "agc_crcf_rssi_sinusoid"},
    {.id =    4, .api = &autotest_agc_crcf_rssi_noise,            .name = "agc_crcf_rssi_noise"},
    {.id =    5, .api = &autotest_cvsd_rmse_sine,                 .name = "cvsd_rmse_sine"},
    {.id =    6, .api = &autotest_cbufferf,                       .name = "cbufferf"},
    {.id =    7, .api = &autotest_cbuffercf,                      .name = "cbuffercf"},
    {.id =    8, .api = &autotest_cbufferf_flow,                  .name = "cbufferf_flow"},
    {.id =    9, .api = &autotest_wdelayf,                        .name = "wdelayf"},
    {.id =   10, .api = &autotest_windowf,                        .name = "windowf"},
    {.id =   11, .api = &autotest_dotprod_rrrf_basic,             .name = "dotprod_rrrf_basic"},
    {.id =   12, .api = &autotest_dotprod_rrrf_uneven,            .name = "dotprod_rrrf_uneven"},
    {.id =   13, .api = &autotest_dotprod_rrrf_struct,            .name = "dotprod_rrrf_struct"},
    {.id =   14, .api = &autotest_dotprod_rrrf_struct_align,      .name = "dotprod_rrrf_struct_align"},
    {.id =   15, .api = &autotest_dotprod_rrrf_rand01,            .name = "dotprod_rrrf_rand01"},
    {.id =   16, .api = &autotest_dotprod_rrrf_rand02,            .name = "dotprod_rrrf_rand02"},
    {.id =   17, .api = &autotest_dotprod_rrrf_struct_lengths,    .name = "dotprod_rrrf_struct_lengths"},
    {.id =   18, .api = &autotest_dotprod_rrrf_struct_vs_ordinal, .name = "dotprod_rrrf_struct_vs_ordinal"},
    {.id =   19, .api = &autotest_dotprod_crcf_rand01,            .name = "dotprod_crcf_rand01"},
    {.id =   20, .api = &autotest_dotprod_crcf_rand02,            .name = "dotprod_crcf_rand02"},
    {.id =   21, .api = &autotest_dotprod_crcf_struct_vs_ordinal, .name = "dotprod_crcf_struct_vs_ordinal"},
    {.id =   22, .api = &autotest_dotprod_cccf_rand16,            .name = "dotprod_cccf_rand16"},
    {.id =   23, .api = &autotest_dotprod_cccf_struct_lengths,    .name = "dotprod_cccf_struct_lengths"},
    {.id =   24, .api = &autotest_dotprod_cccf_struct_vs_ordinal, .name = "dotprod_cccf_struct_vs_ordinal"},
    {.id =   25, .api = &autotest_sumsqf_3,                       .name = "sumsqf_3"},
    {.id =   26, .api = &autotest_sumsqf_4,                       .name = "sumsqf_4"},
    {.id =   27, .api = &autotest_sumsqf_7,                       .name = "sumsqf_7"},
    {.id =   28, .api = &autotest_sumsqf_8,                       .name = "sumsqf_8"},
    {.id =   29, .api = &autotest_sumsqf_15,                      .name = "sumsqf_15"},
    {.id =   30, .api = &autotest_sumsqf_16,                      .name = "sumsqf_16"},
    {.id =   31, .api = &autotest_sumsqcf_3,                      .name = "sumsqcf_3"},
    {.id =   32, .api = &autotest_sumsqcf_4,                      .name = "sumsqcf_4"},
    {.id =   33, .api = &autotest_sumsqcf_7,                      .name = "sumsqcf_7"},
    {.id =   34, .api = &autotest_sumsqcf_8,                      .name = "sumsqcf_8"},
    {.id =   35, .api = &autotest_sumsqcf_15,                     .name = "sumsqcf_15"},
    {.id =   36, .api = &autotest_sumsqcf_16,                     .name = "sumsqcf_16"},
    {.id =   37, .api = &autotest_eqrls_rrrf_01,                  .name = "eqrls_rrrf_01"},
    {.id =   38, .api = &autotest_reverse_byte,                   .name = "reverse_byte"},
    {.id =   39, .api = &autotest_reverse_uint16,                 .name = "reverse_uint16"},
    {.id =   40, .api = &autotest_reverse_uint32,                 .name = "reverse_uint32"},
    {.id =   41, .api = &autotest_checksum,                       .name = "checksum"},
    {.id =   42, .api = &autotest_crc8,                           .name = "crc8"},
    {.id =   43, .api = &autotest_crc16,                          .name = "crc16"},
    {.id =   44, .api = &autotest_crc24,                          .name = "crc24"},
    {.id =   45, .api = &autotest_crc32,                          .name = "crc32"},
    {.id =   46, .api = &autotest_fec_r3,                         .name = "fec_r3"},
    {.id =   47, .api = &autotest_fec_r5,                         .name = "fec_r5"},
    {.id =   48, .api = &autotest_fec_h74,                        .name = "fec_h74"},
    {.id =   49, .api = &autotest_fec_h84,                        .name = "fec_h84"},
    {.id =   50, .api = &autotest_fec_h128,                       .name = "fec_h128"},
    {.id =   51, .api = &autotest_fec_g2412,                      .name = "fec_g2412"},
    {.id =   52, .api = &autotest_fec_secded2216,                 .name = "fec_secded2216"},
    {.id =   53, .api = &autotest_fec_secded3932,                 .name = "fec_secded3932"},
    {.id =   54, .api = &autotest_fec_secded7264,                 .name = "fec_secded7264"},
    {.id =   55, .api = &autotest_fec_v27,                        .name = "fec_v27"},
    {.id =   56, .api = &autotest_fec_v29,                        .name = "fec_v29"},
    {.id =   57, .api = &autotest_fec_v39,                        .name = "fec_v39"},
    {.id =   58, .api = &autotest_fec_v615,                       .name = "fec_v615"},
    {.id =   59, .api = &autotest_fec_v27p23,                     .name = "fec_v27p23"},
    {.id =   60, .api = &autotest_fec_v27p34,                     .name = "fec_v27p34"},
    {.id =   61, .api = &autotest_fec_v27p45,                     .name = "fec_v27p45"},
    {.id =   62, .api = &autotest_fec_v27p56,                     .name = "fec_v27p56"},
    {.id =   63, .api = &autotest_fec_v27p67,                     .name = "fec_v27p67"},
    {.id =   64, .api = &autotest_fec_v27p78,                     .name = "fec_v27p78"},
    {.id =   65, .api = &autotest_fec_v29p23,                     .name = "fec_v29p23"},
    {.id =   66, .api = &autotest_fec_v29p34,                     .name = "fec_v29p34"},
    {.id =   67, .api = &autotest_fec_v29p45,                     .name = "fec_v29p45"},
    {.id =   68, .api = &autotest_fec_v29p56,                     .name = "fec_v29p56"},
    {.id =   69, .api = &autotest_fec_v29p67,                     .name = "fec_v29p67"},
    {.id =   70, .api = &autotest_fec_v29p78,                     .name = "fec_v29p78"},
    {.id =   71, .api = &autotest_fec_rs8,                        .name = "fec_rs8"},
    {.id =   72, .api = &autotest_fecsoft_r3,                     .name = "fecsoft_r3"},
    {.id =   73, .api = &autotest_fecsoft_r5,                     .name = "fecsoft_r5"},
    {.id =   74, .api = &autotest_fecsoft_h74,                    .name = "fecsoft_h74"},
    {.id =   75, .api = &autotest_fecsoft_h84,                    .name = "fecsoft_h84"},
    {.id =   76, .api = &autotest_fecsoft_h128,                   .name = "fecsoft_h128"},
    {.id =   77, .api = &autotest_fecsoft_v27,                    .name = "fecsoft_v27"},
    {.id =   78, .api = &autotest_fecsoft_v29,                    .name = "fecsoft_v29"},
    {.id =   79, .api = &autotest_fecsoft_v39,                    .name = "fecsoft_v39"},
    {.id =   80, .api = &autotest_fecsoft_v615,                   .name = "fecsoft_v615"},
    {.id =   81, .api = &autotest_fecsoft_v27p23,                 .name = "fecsoft_v27p23"},
    {.id =   82, .api = &autotest_fecsoft_v27p34,                 .name = "fecsoft_v27p34"},
    {.id =   83, .api = &autotest_fecsoft_v27p45,                 .name = "fecsoft_v27p45"},
    {.id =   84, .api = &autotest_fecsoft_v27p56,                 .name = "fecsoft_v27p56"},
    {.id =   85, .api = &autotest_fecsoft_v27p67,                 .name = "fecsoft_v27p67"},
    {.id =   86, .api = &autotest_fecsoft_v27p78,                 .name = "fecsoft_v27p78"},
    {.id =   87, .api = &autotest_fecsoft_v29p23,                 .name = "fecsoft_v29p23"},
    {.id =   88, .api = &autotest_fecsoft_v29p34,                 .name = "fecsoft_v29p34"},
    {.id =   89, .api = &autotest_fecsoft_v29p45,                 .name = "fecsoft_v29p45"},
    {.id =   90, .api = &autotest_fecsoft_v29p56,                 .name = "fecsoft_v29p56"},
    {.id =   91, .api = &autotest_fecsoft_v29p67,                 .name = "fecsoft_v29p67"},
    {.id =   92, .api = &autotest_fecsoft_v29p78,                 .name = "fecsoft_v29p78"},
    {.id =   93, .api = &autotest_fecsoft_rs8,                    .name = "fecsoft_rs8"},
    {.id =   94, .api = &autotest_golay2412_codec,                .name = "golay2412_codec"},
    {.id =   95, .api = &autotest_hamming74_codec,                .name = "hamming74_codec"},
    {.id =   96, .api = &autotest_hamming74_codec_soft,           .name = "hamming74_codec_soft"},
    {.id =   97, .api = &autotest_hamming84_codec,                .name = "hamming84_codec"},
    {.id =   98, .api = &autotest_hamming84_codec_soft,           .name = "hamming84_codec_soft"},
    {.id =   99, .api = &autotest_hamming128_codec,               .name = "hamming128_codec"},
    {.id =  100, .api = &autotest_hamming128_codec_soft,          .name = "hamming128_codec_soft"},
    {.id =  101, .api = &autotest_hamming1511_codec,              .name = "hamming1511_codec"},
    {.id =  102, .api = &autotest_hamming3126_codec,              .name = "hamming3126_codec"},
    {.id =  103, .api = &autotest_reedsolomon_223_255,            .name = "reedsolomon_223_255"},
    {.id =  104, .api = &autotest_rep3_codec,                     .name = "rep3_codec"},
    {.id =  105, .api = &autotest_rep5_codec,                     .name = "rep5_codec"},
    {.id =  106, .api = &autotest_secded2216_codec_e0,            .name = "secded2216_codec_e0"},
    {.id =  107, .api = &autotest_secded2216_codec_e1,            .name = "secded2216_codec_e1"},
    {.id =  108, .api = &autotest_secded2216_codec_e2,            .name = "secded2216_codec_e2"},
    {.id =  109, .api = &autotest_secded3932_codec_e0,            .name = "secded3932_codec_e0"},
    {.id =  110, .api = &autotest_secded3932_codec_e1,            .name = "secded3932_codec_e1"},
    {.id =  111, .api = &autotest_secded3932_codec_e2,            .name = "secded3932_codec_e2"},
    {.id =  112, .api = &autotest_secded7264_codec_e0,            .name = "secded7264_codec_e0"},
    {.id =  113, .api = &autotest_secded7264_codec_e1,            .name = "secded7264_codec_e1"},
    {.id =  114, .api = &autotest_secded7264_codec_e2,            .name = "secded7264_codec_e2"},
    {.id =  115, .api = &autotest_interleaver_hard_8,             .name = "interleaver_hard_8"},
    {.id =  116, .api = &autotest_interleaver_hard_16,            .name = "interleaver_hard_16"},
    {.id =  117, .api = &autotest_interleaver_hard_64,            .name = "interleaver_hard_64"},
    {.id =  118, .api = &autotest_interleaver_hard_256,           .name = "interleaver_hard_256"},
    {.id =  119, .api = &autotest_interleaver_soft_8,             .name = "interleaver_soft_8"},
    {.id =  120, .api = &autotest_interleaver_soft_16,            .name = "interleaver_soft_16"},
    {.id =  121, .api = &autotest_interleaver_soft_64,            .name = "interleaver_soft_64"},
    {.id =  122, .api = &autotest_interleaver_soft_256,           .name = "interleaver_soft_256"},
    {.id =  123, .api = &autotest_packetizer_n16_0_0,             .name = "packetizer_n16_0_0"},
    {.id =  124, .api = &autotest_packetizer_n16_0_1,             .name = "packetizer_n16_0_1"},
    {.id =  125, .api = &autotest_packetizer_n16_0_2,             .name = "packetizer_n16_0_2"},
    {.id =  126, .api = &autotest_fft_3,                          .name = "fft_3"},
    {.id =  127, .api = &autotest_fft_5,                          .name = "fft_5"},
    {.id =  128, .api = &autotest_fft_6,                          .name = "fft_6"},
    {.id =  129, .api = &autotest_fft_7,                          .name = "fft_7"},
    {.id =  130, .api = &autotest_fft_9,                          .name = "fft_9"},
    {.id =  131, .api = &autotest_fft_2,                          .name = "fft_2"},
    {.id =  132, .api = &autotest_fft_4,                          .name = "fft_4"},
    {.id =  133, .api = &autotest_fft_8,                          .name = "fft_8"},
    {.id =  134, .api = &autotest_fft_16,                         .name = "fft_16"},
    {.id =  135, .api = &autotest_fft_32,                         .name = "fft_32"},
    {.id =  136, .api = &autotest_fft_64,                         .name = "fft_64"},
    {.id =  137, .api = &autotest_fft_10,                         .name = "fft_10"},
    {.id =  138, .api = &autotest_fft_21,                         .name = "fft_21"},
    {.id =  139, .api = &autotest_fft_22,                         .name = "fft_22"},
    {.id =  140, .api = &autotest_fft_24,                         .name = "fft_24"},
    {.id =  141, .api = &autotest_fft_26,                         .name = "fft_26"},
    {.id =  142, .api = &autotest_fft_30,                         .name = "fft_30"},
    {.id =  143, .api = &autotest_fft_35,                         .name = "fft_35"},
    {.id =  144, .api = &autotest_fft_36,                         .name = "fft_36"},
    {.id =  145, .api = &autotest_fft_48,                         .name = "fft_48"},
    {.id =  146, .api = &autotest_fft_63,                         .name = "fft_63"},
    {.id =  147, .api = &autotest_fft_92,                         .name = "fft_92"},
    {.id =  148, .api = &autotest_fft_96,                         .name = "fft_96"},
    {.id =  149, .api = &autotest_fft_120,                        .name = "fft_120"},
    {.id =  150, .api = &autotest_fft_130,                        .name = "fft_130"},
    {.id =  151, .api = &autotest_fft_192,                        .name = "fft_192"},
    {.id =  152, .api = &autotest_fft_17,                         .name = "fft_17"},
    {.id =  153, .api = &autotest_fft_43,                         .name = "fft_43"},
    {.id =  154, .api = &autotest_fft_79,                         .name = "fft_79"},
    {.id =  155, .api = &autotest_fft_157,                        .name = "fft_157"},
    {.id =  156, .api = &autotest_fft_317,                        .name = "fft_317"},
    {.id =  157, .api = &autotest_fft_509,                        .name = "fft_509"},
    {.id =  158, .api = &autotest_fft_r2r_REDFT00_n8,             .name = "fft_r2r_REDFT00_n8"},
    {.id =  159, .api = &autotest_fft_r2r_REDFT10_n8,             .name = "fft_r2r_REDFT10_n8"},
    {.id =  160, .api = &autotest_fft_r2r_REDFT01_n8,             .name = "fft_r2r_REDFT01_n8"},
    {.id =  161, .api = &autotest_fft_r2r_REDFT11_n8,             .name = "fft_r2r_REDFT11_n8"},
    {.id =  162, .api = &autotest_fft_r2r_RODFT00_n8,             .name = "fft_r2r_RODFT00_n8"},
    {.id =  163, .api = &autotest_fft_r2r_RODFT10_n8,             .name = "fft_r2r_RODFT10_n8"},
    {.id =  164, .api = &autotest_fft_r2r_RODFT01_n8,             .name = "fft_r2r_RODFT01_n8"},
    {.id =  165, .api = &autotest_fft_r2r_RODFT11_n8,             .name = "fft_r2r_RODFT11_n8"},
    {.id =  166, .api = &autotest_fft_r2r_REDFT00_n32,            .name = "fft_r2r_REDFT00_n32"},
    {.id =  167, .api = &autotest_fft_r2r_REDFT10_n32,            .name = "fft_r2r_REDFT10_n32"},
    {.id =  168, .api = &autotest_fft_r2r_REDFT01_n32,            .name = "fft_r2r_REDFT01_n32"},
    {.id =  169, .api = &autotest_fft_r2r_REDFT11_n32,            .name = "fft_r2r_REDFT11_n32"},
    {.id =  170, .api = &autotest_fft_r2r_RODFT00_n32,            .name = "fft_r2r_RODFT00_n32"},
    {.id =  171, .api = &autotest_fft_r2r_RODFT10_n32,            .name = "fft_r2r_RODFT10_n32"},
    {.id =  172, .api = &autotest_fft_r2r_RODFT01_n32,            .name = "fft_r2r_RODFT01_n32"},
    {.id =  173, .api = &autotest_fft_r2r_RODFT11_n32,            .name = "fft_r2r_RODFT11_n32"},
    {.id =  174, .api = &autotest_fft_r2r_REDFT00_n27,            .name = "fft_r2r_REDFT00_n27"},
    {.id =  175, .api = &autotest_fft_r2r_REDFT10_n27,            .name = "fft_r2r_REDFT10_n27"},
    {.id =  176, .api = &autotest_fft_r2r_REDFT01_n27,            .name = "fft_r2r_REDFT01_n27"},
    {.id =  177, .api = &autotest_fft_r2r_REDFT11_n27,            .name = "fft_r2r_REDFT11_n27"},
    {.id =  178, .api = &autotest_fft_r2r_RODFT00_n27,            .name = "fft_r2r_RODFT00_n27"},
    {.id =  179, .api = &autotest_fft_r2r_RODFT10_n27,            .name = "fft_r2r_RODFT10_n27"},
    {.id =  180, .api = &autotest_fft_r2r_RODFT01_n27,            .name = "fft_r2r_RODFT01_n27"},
    {.id =  181, .api = &autotest_fft_r2r_RODFT11_n27,            .name = "fft_r2r_RODFT11_n27"},
    {.id =  182, .api = &autotest_fft_shift_4,                    .name = "fft_shift_4"},
    {.id =  183, .api = &autotest_fft_shift_8,                    .name = "fft_shift_8"},
    {.id =  184, .api = &autotest_fftfilt_rrrf_data_h4x256,       .name = "fftfilt_rrrf_data_h4x256"},
    {.id =  185, .api = &autotest_fftfilt_rrrf_data_h7x256,       .name = "fftfilt_rrrf_data_h7x256"},
    {.id =  186, .api = &autotest_fftfilt_rrrf_data_h13x256,      .name = "fftfilt_rrrf_data_h13x256"},
    {.id =  187, .api = &autotest_fftfilt_rrrf_data_h23x256,      .name = "fftfilt_rrrf_data_h23x256"},
    {.id =  188, .api = &autotest_fftfilt_crcf_data_h4x256,       .name = "fftfilt_crcf_data_h4x256"},
    {.id =  189, .api = &autotest_fftfilt_crcf_data_h7x256,       .name = "fftfilt_crcf_data_h7x256"},
    {.id =  190, .api = &autotest_fftfilt_crcf_data_h13x256,      .name = "fftfilt_crcf_data_h13x256"},
    {.id =  191, .api = &autotest_fftfilt_crcf_data_h23x256,      .name = "fftfilt_crcf_data_h23x256"},
    {.id =  192, .api = &autotest_fftfilt_cccf_data_h4x256,       .name = "fftfilt_cccf_data_h4x256"},
    {.id =  193, .api = &autotest_fftfilt_cccf_data_h7x256,       .name = "fftfilt_cccf_data_h7x256"},
    {.id =  194, .api = &autotest_fftfilt_cccf_data_h13x256,      .name = "fftfilt_cccf_data_h13x256"},
    {.id =  195, .api = &autotest_fftfilt_cccf_data_h23x256,      .name = "fftfilt_cccf_data_h23x256"},
    {.id =  196, .api = &autotest_filter_crosscorr_rrrf,          .name = "filter_crosscorr_rrrf"},
    {.id =  197, .api = &autotest_firdecim_rrrf_data_M2h4x20,     .name = "firdecim_rrrf_data_M2h4x20"},
    {.id =  198, .api = &autotest_firdecim_rrrf_data_M3h7x30,     .name = "firdecim_rrrf_data_M3h7x30"},
    {.id =  199, .api = &autotest_firdecim_rrrf_data_M4h13x40,    .name = "firdecim_rrrf_data_M4h13x40"},
    {.id =  200, .api = &autotest_firdecim_rrrf_data_M5h23x50,    .name = "firdecim_rrrf_data_M5h23x50"},
    {.id =  201, .api = &autotest_firdecim_crcf_data_M2h4x20,     .name = "firdecim_crcf_data_M2h4x20"},
    {.id =  202, .api = &autotest_firdecim_crcf_data_M3h7x30,     .name = "firdecim_crcf_data_M3h7x30"},
    {.id =  203, .api = &autotest_firdecim_crcf_data_M4h13x40,    .name = "firdecim_crcf_data_M4h13x40"},
    {.id =  204, .api = &autotest_firdecim_crcf_data_M5h23x50,    .name = "firdecim_crcf_data_M5h23x50"},
    {.id =  205, .api = &autotest_firdecim_cccf_data_M2h4x20,     .name = "firdecim_cccf_data_M2h4x20"},
    {.id =  206, .api = &autotest_firdecim_cccf_data_M3h7x30,     .name = "firdecim_cccf_data_M3h7x30"},
    {.id =  207, .api = &autotest_firdecim_cccf_data_M4h13x40,    .name = "firdecim_cccf_data_M4h13x40"},
    {.id =  208, .api = &autotest_firdecim_cccf_data_M5h23x50,    .name = "firdecim_cccf_data_M5h23x50"},
    {.id =  209, .api = &autotest_liquid_firdes_rcos,             .name = "liquid_firdes_rcos"},
    {.id =  210, .api = &autotest_liquid_firdes_rrcos,            .name = "liquid_firdes_rrcos"},
    {.id =  211, .api = &autotest_liquid_firdes_rkaiser,          .name = "liquid_firdes_rkaiser"},
    {.id =  212, .api = &autotest_firdespm_bandpass_n24,          .name = "firdespm_bandpass_n24"},
    {.id =  213, .api = &autotest_firdespm_bandpass_n32,          .name = "firdespm_bandpass_n32"},
    {.id =  214, .api = &autotest_firfilt_rrrf_data_h4x8,         .name = "firfilt_rrrf_data_h4x8"},
    {.id =  215, .api = &autotest_firfilt_rrrf_data_h7x16,        .name = "firfilt_rrrf_data_h7x16"},
    {.id =  216, .api = &autotest_firfilt_rrrf_data_h13x32,       .name = "firfilt_rrrf_data_h13x32"},
    {.id =  217, .api = &autotest_firfilt_rrrf_data_h23x64,       .name = "firfilt_rrrf_data_h23x64"},
    {.id =  218, .api = &autotest_firfilt_crcf_data_h4x8,         .name = "firfilt_crcf_data_h4x8"},
    {.id =  219, .api = &autotest_firfilt_crcf_data_h7x16,        .name = "firfilt_crcf_data_h7x16"},
    {.id =  220, .api = &autotest_firfilt_crcf_data_h13x32,       .name = "firfilt_crcf_data_h13x32"},
    {.id =  221, .api = &autotest_firfilt_crcf_data_h23x64,       .name = "firfilt_crcf_data_h23x64"},
    {.id =  222, .api = &autotest_firfilt_cccf_data_h4x8,         .name = "firfilt_cccf_data_h4x8"},
    {.id =  223, .api = &autotest_firfilt_cccf_data_h7x16,        .name = "firfilt_cccf_data_h7x16"},
    {.id =  224, .api = &autotest_firfilt_cccf_data_h13x32,       .name = "firfilt_cccf_data_h13x32"},
    {.id =  225, .api = &autotest_firfilt_cccf_data_h23x64,       .name = "firfilt_cccf_data_h23x64"},
    {.id =  226, .api = &autotest_firhilbf_decim,                 .name = "firhilbf_decim"},
    {.id =  227, .api = &autotest_firhilbf_interp,                .name = "firhilbf_interp"},
    {.id =  228, .api = &autotest_firinterp_rrrf_generic,         .name = "firinterp_rrrf_generic"},
    {.id =  229, .api = &autotest_firinterp_crcf_generic,         .name = "firinterp_crcf_generic"},
    {.id =  230, .api = &autotest_firpfb_impulse_response,        .name = "firpfb_impulse_response"},
    {.id =  231, .api = &autotest_fir_groupdelay_n3,              .name = "fir_groupdelay_n3"},
    {.id =  232, .api = &autotest_iir_groupdelay_n3,              .name = "iir_groupdelay_n3"},
    {.id =  233, .api = &autotest_iir_groupdelay_n8,              .name = "iir_groupdelay_n8"},
    {.id =  234, .api = &autotest_iir_groupdelay_sos_n8,          .name = "iir_groupdelay_sos_n8"},
    {.id =  235, .api = &autotest_iirdes_butter_2,                .name = "iirdes_butter_2"},
    {.id =  236, .api = &autotest_iirdes_cplxpair_n6,             .name = "iirdes_cplxpair_n6"},
    {.id =  237, .api = &autotest_iirdes_cplxpair_n20,            .name = "iirdes_cplxpair_n20"},
    {.id =  238, .api = &autotest_iirdes_dzpk2sosf,               .name = "iirdes_dzpk2sosf"},
    {.id =  239, .api = &autotest_iirdes_isstable_n2_yes,         .name = "iirdes_isstable_n2_yes"},
    {.id =  240, .api = &autotest_iirdes_isstable_n2_no,          .name = "iirdes_isstable_n2_no"},
    {.id =  241, .api = &autotest_iirfilt_rrrf_h3x64,             .name = "iirfilt_rrrf_h3x64"},
    {.id =  242, .api = &autotest_iirfilt_rrrf_h5x64,             .name = "iirfilt_rrrf_h5x64"},
    {.id =  243, .api = &autotest_iirfilt_rrrf_h7x64,             .name = "iirfilt_rrrf_h7x64"},
    {.id =  244, .api = &autotest_iirfilt_crcf_h3x64,             .name = "iirfilt_crcf_h3x64"},
    {.id =  245, .api = &autotest_iirfilt_crcf_h5x64,             .name = "iirfilt_crcf_h5x64"},
    {.id =  246, .api = &autotest_iirfilt_crcf_h7x64,             .name = "iirfilt_crcf_h7x64"},
    {.id =  247, .api = &autotest_iirfilt_cccf_h3x64,             .name = "iirfilt_cccf_h3x64"},
    {.id =  248, .api = &autotest_iirfilt_cccf_h5x64,             .name = "iirfilt_cccf_h5x64"},
    {.id =  249, .api = &autotest_iirfilt_cccf_h7x64,             .name = "iirfilt_cccf_h7x64"},
    {.id =  250, .api = &autotest_iirfiltsos_impulse_n2,          .name = "iirfiltsos_impulse_n2"},
    {.id =  251, .api = &autotest_iirfiltsos_step_n2,             .name = "iirfiltsos_step_n2"},
    {.id =  252, .api = &autotest_msresamp_crcf,                  .name = "msresamp_crcf"},
    {.id =  253, .api = &autotest_resamp_crcf,                    .name = "resamp_crcf"},
    {.id =  254, .api = &autotest_resamp2_analysis,               .name = "resamp2_analysis"},
    {.id =  255, .api = &autotest_resamp2_synthesis,              .name = "resamp2_synthesis"},
    {.id =  256, .api = &autotest_symsync_crcf_scenario_0,        .name = "symsync_crcf_scenario_0"},
    {.id =  257, .api = &autotest_symsync_crcf_scenario_1,        .name = "symsync_crcf_scenario_1"},
    {.id =  258, .api = &autotest_symsync_crcf_scenario_2,        .name = "symsync_crcf_scenario_2"},
    {.id =  259, .api = &autotest_symsync_crcf_scenario_3,        .name = "symsync_crcf_scenario_3"},
    {.id =  260, .api = &autotest_symsync_rrrf_scenario_0,        .name = "symsync_rrrf_scenario_0"},
    {.id =  261, .api = &autotest_symsync_rrrf_scenario_1,        .name = "symsync_rrrf_scenario_1"},
    {.id =  262, .api = &autotest_symsync_rrrf_scenario_2,        .name = "symsync_rrrf_scenario_2"},
    {.id =  263, .api = &autotest_symsync_rrrf_scenario_3,        .name = "symsync_rrrf_scenario_3"},
    {.id =  264, .api = &autotest_bpacketsync,                    .name = "bpacketsync"},
    {.id =  265, .api = &autotest_bsync_rrrf_15,                  .name = "bsync_rrrf_15"},
    {.id =  266, .api = &autotest_bsync_crcf_15,                  .name = "bsync_crcf_15"},
    {.id =  267, .api = &autotest_detector_cccf_n64,              .name = "detector_cccf_n64"},
    {.id =  268, .api = &autotest_detector_cccf_n83,              .name = "detector_cccf_n83"},
    {.id =  269, .api = &autotest_detector_cccf_n128,             .name = "detector_cccf_n128"},
    {.id =  270, .api = &autotest_detector_cccf_n167,             .name = "detector_cccf_n167"},
    {.id =  271, .api = &autotest_detector_cccf_n256,             .name = "detector_cccf_n256"},
    {.id =  272, .api = &autotest_detector_cccf_n335,             .name = "detector_cccf_n335"},
    {.id =  273, .api = &autotest_detector_cccf_n512,             .name = "detector_cccf_n512"},
    {.id =  274, .api = &autotest_detector_cccf_n671,             .name = "detector_cccf_n671"},
    {.id =  275, .api = &autotest_detector_cccf_n1024,            .name = "detector_cccf_n1024"},
    {.id =  276, .api = &autotest_detector_cccf_n1341,            .name = "detector_cccf_n1341"},
    {.id =  277, .api = &autotest_flexframesync,                  .name = "flexframesync"},
    {.id =  278, .api = &autotest_framesync64,                    .name = "framesync64"},
    {.id =  279, .api = &autotest_qdetector_cccf_linear_n64,      .name = "qdetector_cccf_linear_n64"},
    {.id =  280, .api = &autotest_qdetector_cccf_linear_n83,      .name = "qdetector_cccf_linear_n83"},
    {.id =  281, .api = &autotest_qdetector_cccf_linear_n128,     .name = "qdetector_cccf_linear_n128"},
    {.id =  282, .api = &autotest_qdetector_cccf_linear_n167,     .name = "qdetector_cccf_linear_n167"},
    {.id =  283, .api = &autotest_qdetector_cccf_linear_n256,     .name = "qdetector_cccf_linear_n256"},
    {.id =  284, .api = &autotest_qdetector_cccf_linear_n335,     .name = "qdetector_cccf_linear_n335"},
    {.id =  285, .api = &autotest_qdetector_cccf_linear_n512,     .name = "qdetector_cccf_linear_n512"},
    {.id =  286, .api = &autotest_qdetector_cccf_linear_n671,     .name = "qdetector_cccf_linear_n671"},
    {.id =  287, .api = &autotest_qdetector_cccf_linear_n1024,    .name = "qdetector_cccf_linear_n1024"},
    {.id =  288, .api = &autotest_qdetector_cccf_linear_n1341,    .name = "qdetector_cccf_linear_n1341"},
    {.id =  289, .api = &autotest_qdetector_cccf_gmsk_n64,        .name = "qdetector_cccf_gmsk_n64"},
    {.id =  290, .api = &autotest_qdetector_cccf_gmsk_n83,        .name = "qdetector_cccf_gmsk_n83"},
    {.id =  291, .api = &autotest_qdetector_cccf_gmsk_n128,       .name = "qdetector_cccf_gmsk_n128"},
    {.id =  292, .api = &autotest_qdetector_cccf_gmsk_n167,       .name = "qdetector_cccf_gmsk_n167"},
    {.id =  293, .api = &autotest_qdetector_cccf_gmsk_n256,       .name = "qdetector_cccf_gmsk_n256"},
    {.id =  294, .api = &autotest_qdetector_cccf_gmsk_n335,       .name = "qdetector_cccf_gmsk_n335"},
    {.id =  295, .api = &autotest_qdetector_cccf_gmsk_n512,       .name = "qdetector_cccf_gmsk_n512"},
    {.id =  296, .api = &autotest_qdetector_cccf_gmsk_n671,       .name = "qdetector_cccf_gmsk_n671"},
    {.id =  297, .api = &autotest_qdetector_cccf_gmsk_n1024,      .name = "qdetector_cccf_gmsk_n1024"},
    {.id =  298, .api = &autotest_qdetector_cccf_gmsk_n1341,      .name = "qdetector_cccf_gmsk_n1341"},
    {.id =  299, .api = &autotest_qpacketmodem_bpsk,              .name = "qpacketmodem_bpsk"},
    {.id =  300, .api = &autotest_qpacketmodem_qpsk,              .name = "qpacketmodem_qpsk"},
    {.id =  301, .api = &autotest_qpacketmodem_psk8,              .name = "qpacketmodem_psk8"},
    {.id =  302, .api = &autotest_qpacketmodem_qam16,             .name = "qpacketmodem_qam16"},
    {.id =  303, .api = &autotest_qpacketmodem_sqam32,            .name = "qpacketmodem_sqam32"},
    {.id =  304, .api = &autotest_qpacketmodem_qam64,             .name = "qpacketmodem_qam64"},
    {.id =  305, .api = &autotest_qpacketmodem_sqam128,           .name = "qpacketmodem_sqam128"},
    {.id =  306, .api = &autotest_qpacketmodem_qam256,            .name = "qpacketmodem_qam256"},
    {.id =  307, .api = &autotest_qpilotsync_100_16,              .name = "qpilotsync_100_16"},
    {.id =  308, .api = &autotest_qpilotsync_200_20,              .name = "qpilotsync_200_20"},
    {.id =  309, .api = &autotest_qpilotsync_300_24,              .name = "qpilotsync_300_24"},
    {.id =  310, .api = &autotest_qpilotsync_400_28,              .name = "qpilotsync_400_28"},
    {.id =  311, .api = &autotest_qpilotsync_500_32,              .name = "qpilotsync_500_32"},
    {.id =  312, .api = &autotest_kbd_n16,                        .name = "kbd_n16"},
    {.id =  313, .api = &autotest_kbd_n32,                        .name = "kbd_n32"},
    {.id =  314, .api = &autotest_kbd_n48,                        .name = "kbd_n48"},
    {.id =  315, .api = &autotest_Q,                              .name = "Q"},
    {.id =  316, .api = &autotest_sincf,                          .name = "sincf"},
    {.id =  317, .api = &autotest_nextpow2,                       .name = "nextpow2"},
    {.id =  318, .api = &autotest_lnbesselif,                     .name = "lnbesselif"},
    {.id =  319, .api = &autotest_besselif,                       .name = "besselif"},
    {.id =  320, .api = &autotest_besseli0f,                      .name = "besseli0f"},
    {.id =  321, .api = &autotest_besseljf,                       .name = "besseljf"},
    {.id =  322, .api = &autotest_besselj0f,                      .name = "besselj0f"},
    {.id =  323, .api = &autotest_gamma,                          .name = "gamma"},
    {.id =  324, .api = &autotest_lngamma,                        .name = "lngamma"},
    {.id =  325, .api = &autotest_uppergamma,                     .name = "uppergamma"},
    {.id =  326, .api = &autotest_factorial,                      .name = "factorial"},
    {.id =  327, .api = &autotest_nchoosek,                       .name = "nchoosek"},
    {.id =  328, .api = &autotest_cexpf,                          .name = "cexpf"},
    {.id =  329, .api = &autotest_clogf,                          .name = "clogf"},
    {.id =  330, .api = &autotest_csqrtf,                         .name = "csqrtf"},
    {.id =  331, .api = &autotest_casinf,                         .name = "casinf"},
    {.id =  332, .api = &autotest_cacosf,                         .name = "cacosf"},
    {.id =  333, .api = &autotest_catanf,                         .name = "catanf"},
    {.id =  334, .api = &autotest_polyf_fit_q3n3,                 .name = "polyf_fit_q3n3"},
    {.id =  335, .api = &autotest_polyf_expandroots_4,            .name = "polyf_expandroots_4"},
    {.id =  336, .api = &autotest_polyf_expandroots_11,           .name = "polyf_expandroots_11"},
    {.id =  337, .api = &autotest_polycf_expandroots_4,           .name = "polycf_expandroots_4"},
    {.id =  338, .api = &autotest_polyf_expandroots2_3,           .name = "polyf_expandroots2_3"},
    {.id =  339, .api = &autotest_polyf_mul_2_3,                  .name = "polyf_mul_2_3"},
    {.id =  340, .api = &autotest_poly_expandbinomial_n6,         .name = "poly_expandbinomial_n6"},
    {.id =  341, .api = &autotest_poly_binomial_expand_pm_m6_k1,  .name = "poly_binomial_expand_pm_m6_k1"},
    {.id =  342, .api = &autotest_poly_expandbinomial_pm_m5_k2,   .name = "poly_expandbinomial_pm_m5_k2"},
    {.id =  343, .api = &autotest_polyf_findroots,                .name = "polyf_findroots"},
    {.id =  344, .api = &autotest_matrixcf_add,                   .name = "matrixcf_add"},
    {.id =  345, .api = &autotest_matrixcf_aug,                   .name = "matrixcf_aug"},
    {.id =  346, .api = &autotest_matrixcf_chol,                  .name = "matrixcf_chol"},
    {.id =  347, .api = &autotest_matrixcf_inv,                   .name = "matrixcf_inv"},
    {.id =  348, .api = &autotest_matrixcf_linsolve,              .name = "matrixcf_linsolve"},
    {.id =  349, .api = &autotest_matrixcf_ludecomp_crout,        .name = "matrixcf_ludecomp_crout"},
    {.id =  350, .api = &autotest_matrixcf_ludecomp_doolittle,    .name = "matrixcf_ludecomp_doolittle"},
    {.id =  351, .api = &autotest_matrixcf_mul,                   .name = "matrixcf_mul"},
    {.id =  352, .api = &autotest_matrixcf_qrdecomp,              .name = "matrixcf_qrdecomp"},
    {.id =  353, .api = &autotest_matrixcf_transmul,              .name = "matrixcf_transmul"},
    {.id =  354, .api = &autotest_matrixf_add,                    .name = "matrixf_add"},
    {.id =  355, .api = &autotest_matrixf_aug,                    .name = "matrixf_aug"},
    {.id =  356, .api = &autotest_matrixf_cgsolve,                .name = "matrixf_cgsolve"},
    {.id =  357, .api = &autotest_matrixf_chol,                   .name = "matrixf_chol"},
    {.id =  358, .api = &autotest_matrixf_gramschmidt,            .name = "matrixf_gramschmidt"},
    {.id =  359, .api = &autotest_matrixf_inv,                    .name = "matrixf_inv"},
    {.id =  360, .api = &autotest_matrixf_linsolve,               .name = "matrixf_linsolve"},
    {.id =  361, .api = &autotest_matrixf_ludecomp_crout,         .name = "matrixf_ludecomp_crout"},
    {.id =  362, .api = &autotest_matrixf_ludecomp_doolittle,     .name = "matrixf_ludecomp_doolittle"},
    {.id =  363, .api = &autotest_matrixf_mul,                    .name = "matrixf_mul"},
    {.id =  364, .api = &autotest_matrixf_qrdecomp,               .name = "matrixf_qrdecomp"},
    {.id =  365, .api = &autotest_matrixf_transmul,               .name = "matrixf_transmul"},
    {.id =  366, .api = &autotest_smatrixb_vmul,                  .name = "smatrixb_vmul"},
    {.id =  367, .api = &autotest_smatrixb_mul,                   .name = "smatrixb_mul"},
    {.id =  368, .api = &autotest_smatrixb_mulf,                  .name = "smatrixb_mulf"},
    {.id =  369, .api = &autotest_smatrixb_vmulf,                 .name = "smatrixb_vmulf"},
    {.id =  370, .api = &autotest_smatrixf_vmul,                  .name = "smatrixf_vmul"},
    {.id =  371, .api = &autotest_smatrixf_mul,                   .name = "smatrixf_mul"},
    {.id =  372, .api = &autotest_smatrixi_vmul,                  .name = "smatrixi_vmul"},
    {.id =  373, .api = &autotest_smatrixi_mul,                   .name = "smatrixi_mul"},
    {.id =  374, .api = &autotest_freqmodem_kf_0_02,              .name = "freqmodem_kf_0_02"},
    {.id =  375, .api = &autotest_freqmodem_kf_0_04,              .name = "freqmodem_kf_0_04"},
    {.id =  376, .api = &autotest_freqmodem_kf_0_08,              .name = "freqmodem_kf_0_08"},
    {.id =  377, .api = &autotest_mod_demod_psk2,                 .name = "mod_demod_psk2"},
    {.id =  378, .api = &autotest_mod_demod_psk4,                 .name = "mod_demod_psk4"},
    {.id =  379, .api = &autotest_mod_demod_psk8,                 .name = "mod_demod_psk8"},
    {.id =  380, .api = &autotest_mod_demod_psk16,                .name = "mod_demod_psk16"},
    {.id =  381, .api = &autotest_mod_demod_psk32,                .name = "mod_demod_psk32"},
    {.id =  382, .api = &autotest_mod_demod_psk64,                .name = "mod_demod_psk64"},
    {.id =  383, .api = &autotest_mod_demod_psk128,               .name = "mod_demod_psk128"},
    {.id =  384, .api = &autotest_mod_demod_psk256,               .name = "mod_demod_psk256"},
    {.id =  385, .api = &autotest_mod_demod_dpsk2,                .name = "mod_demod_dpsk2"},
    {.id =  386, .api = &autotest_mod_demod_dpsk4,                .name = "mod_demod_dpsk4"},
    {.id =  387, .api = &autotest_mod_demod_dpsk8,                .name = "mod_demod_dpsk8"},
    {.id =  388, .api = &autotest_mod_demod_dpsk16,               .name = "mod_demod_dpsk16"},
    {.id =  389, .api = &autotest_mod_demod_dpsk32,               .name = "mod_demod_dpsk32"},
    {.id =  390, .api = &autotest_mod_demod_dpsk64,               .name = "mod_demod_dpsk64"},
    {.id =  391, .api = &autotest_mod_demod_dpsk128,              .name = "mod_demod_dpsk128"},
    {.id =  392, .api = &autotest_mod_demod_dpsk256,              .name = "mod_demod_dpsk256"},
    {.id =  393, .api = &autotest_mod_demod_ask2,                 .name = "mod_demod_ask2"},
    {.id =  394, .api = &autotest_mod_demod_ask4,                 .name = "mod_demod_ask4"},
    {.id =  395, .api = &autotest_mod_demod_ask8,                 .name = "mod_demod_ask8"},
    {.id =  396, .api = &autotest_mod_demod_ask16,                .name = "mod_demod_ask16"},
    {.id =  397, .api = &autotest_mod_demod_ask32,                .name = "mod_demod_ask32"},
    {.id =  398, .api = &autotest_mod_demod_ask64,                .name = "mod_demod_ask64"},
    {.id =  399, .api = &autotest_mod_demod_ask128,               .name = "mod_demod_ask128"},
    {.id =  400, .api = &autotest_mod_demod_ask256,               .name = "mod_demod_ask256"},
    {.id =  401, .api = &autotest_mod_demod_qam4,                 .name = "mod_demod_qam4"},
    {.id =  402, .api = &autotest_mod_demod_qam8,                 .name = "mod_demod_qam8"},
    {.id =  403, .api = &autotest_mod_demod_qam16,                .name = "mod_demod_qam16"},
    {.id =  404, .api = &autotest_mod_demod_qam32,                .name = "mod_demod_qam32"},
    {.id =  405, .api = &autotest_mod_demod_qam64,                .name = "mod_demod_qam64"},
    {.id =  406, .api = &autotest_mod_demod_qam128,               .name = "mod_demod_qam128"},
    {.id =  407, .api = &autotest_mod_demod_qam256,               .name = "mod_demod_qam256"},
    {.id =  408, .api = &autotest_mod_demod_apsk4,                .name = "mod_demod_apsk4"},
    {.id =  409, .api = &autotest_mod_demod_apsk8,                .name = "mod_demod_apsk8"},
    {.id =  410, .api = &autotest_mod_demod_apsk16,               .name = "mod_demod_apsk16"},
    {.id =  411, .api = &autotest_mod_demod_apsk32,               .name = "mod_demod_apsk32"},
    {.id =  412, .api = &autotest_mod_demod_apsk64,               .name = "mod_demod_apsk64"},
    {.id =  413, .api = &autotest_mod_demod_apsk128,              .name = "mod_demod_apsk128"},
    {.id =  414, .api = &autotest_mod_demod_apsk256,              .name = "mod_demod_apsk256"},
    {.id =  415, .api = &autotest_mod_demod_bpsk,                 .name = "mod_demod_bpsk"},
    {.id =  416, .api = &autotest_mod_demod_qpsk,                 .name = "mod_demod_qpsk"},
    {.id =  417, .api = &autotest_mod_demod_ook,                  .name = "mod_demod_ook"},
    {.id =  418, .api = &autotest_mod_demod_sqam32,               .name = "mod_demod_sqam32"},
    {.id =  419, .api = &autotest_mod_demod_sqam128,              .name = "mod_demod_sqam128"},
    {.id =  420, .api = &autotest_mod_demod_V29,                  .name = "mod_demod_V29"},
    {.id =  421, .api = &autotest_mod_demod_arb16opt,             .name = "mod_demod_arb16opt"},
    {.id =  422, .api = &autotest_mod_demod_arb32opt,             .name = "mod_demod_arb32opt"},
    {.id =  423, .api = &autotest_mod_demod_arb64opt,             .name = "mod_demod_arb64opt"},
    {.id =  424, .api = &autotest_mod_demod_arb128opt,            .name = "mod_demod_arb128opt"},
    {.id =  425, .api = &autotest_mod_demod_arb256opt,            .name = "mod_demod_arb256opt"},
    {.id =  426, .api = &autotest_mod_demod_arb64vt,              .name = "mod_demod_arb64vt"},
    {.id =  427, .api = &autotest_demodsoft_psk2,                 .name = "demodsoft_psk2"},
    {.id =  428, .api = &autotest_demodsoft_psk4,                 .name = "demodsoft_psk4"},
    {.id =  429, .api = &autotest_demodsoft_psk8,                 .name = "demodsoft_psk8"},
    {.id =  430, .api = &autotest_demodsoft_psk16,                .name = "demodsoft_psk16"},
    {.id =  431, .api = &autotest_demodsoft_psk32,                .name = "demodsoft_psk32"},
    {.id =  432, .api = &autotest_demodsoft_psk64,                .name = "demodsoft_psk64"},
    {.id =  433, .api = &autotest_demodsoft_psk128,               .name = "demodsoft_psk128"},
    {.id =  434, .api = &autotest_demodsoft_psk256,               .name = "demodsoft_psk256"},
    {.id =  435, .api = &autotest_demodsoft_dpsk2,                .name = "demodsoft_dpsk2"},
    {.id =  436, .api = &autotest_demodsoft_dpsk4,                .name = "demodsoft_dpsk4"},
    {.id =  437, .api = &autotest_demodsoft_dpsk8,                .name = "demodsoft_dpsk8"},
    {.id =  438, .api = &autotest_demodsoft_dpsk16,               .name = "demodsoft_dpsk16"},
    {.id =  439, .api = &autotest_demodsoft_dpsk32,               .name = "demodsoft_dpsk32"},
    {.id =  440, .api = &autotest_demodsoft_dpsk64,               .name = "demodsoft_dpsk64"},
    {.id =  441, .api = &autotest_demodsoft_dpsk128,              .name = "demodsoft_dpsk128"},
    {.id =  442, .api = &autotest_demodsoft_dpsk256,              .name = "demodsoft_dpsk256"},
    {.id =  443, .api = &autotest_demodsoft_ask2,                 .name = "demodsoft_ask2"},
    {.id =  444, .api = &autotest_demodsoft_ask4,                 .name = "demodsoft_ask4"},
    {.id =  445, .api = &autotest_demodsoft_ask8,                 .name = "demodsoft_ask8"},
    {.id =  446, .api = &autotest_demodsoft_ask16,                .name = "demodsoft_ask16"},
    {.id =  447, .api = &autotest_demodsoft_ask32,                .name = "demodsoft_ask32"},
    {.id =  448, .api = &autotest_demodsoft_ask64,                .name = "demodsoft_ask64"},
    {.id =  449, .api = &autotest_demodsoft_ask128,               .name = "demodsoft_ask128"},
    {.id =  450, .api = &autotest_demodsoft_ask256,               .name = "demodsoft_ask256"},
    {.id =  451, .api = &autotest_demodsoft_qam4,                 .name = "demodsoft_qam4"},
    {.id =  452, .api = &autotest_demodsoft_qam8,                 .name = "demodsoft_qam8"},
    {.id =  453, .api = &autotest_demodsoft_qam16,                .name = "demodsoft_qam16"},
    {.id =  454, .api = &autotest_demodsoft_qam32,                .name = "demodsoft_qam32"},
    {.id =  455, .api = &autotest_demodsoft_qam64,                .name = "demodsoft_qam64"},
    {.id =  456, .api = &autotest_demodsoft_qam128,               .name = "demodsoft_qam128"},
    {.id =  457, .api = &autotest_demodsoft_qam256,               .name = "demodsoft_qam256"},
    {.id =  458, .api = &autotest_demodsoft_apsk4,                .name = "demodsoft_apsk4"},
    {.id =  459, .api = &autotest_demodsoft_apsk8,                .name = "demodsoft_apsk8"},
    {.id =  460, .api = &autotest_demodsoft_apsk16,               .name = "demodsoft_apsk16"},
    {.id =  461, .api = &autotest_demodsoft_apsk32,               .name = "demodsoft_apsk32"},
    {.id =  462, .api = &autotest_demodsoft_apsk64,               .name = "demodsoft_apsk64"},
    {.id =  463, .api = &autotest_demodsoft_apsk128,              .name = "demodsoft_apsk128"},
    {.id =  464, .api = &autotest_demodsoft_apsk256,              .name = "demodsoft_apsk256"},
    {.id =  465, .api = &autotest_demodsoft_bpsk,                 .name = "demodsoft_bpsk"},
    {.id =  466, .api = &autotest_demodsoft_qpsk,                 .name = "demodsoft_qpsk"},
    {.id =  467, .api = &autotest_demodsoft_ook,                  .name = "demodsoft_ook"},
    {.id =  468, .api = &autotest_demodsoft_sqam32,               .name = "demodsoft_sqam32"},
    {.id =  469, .api = &autotest_demodsoft_sqam128,              .name = "demodsoft_sqam128"},
    {.id =  470, .api = &autotest_demodsoft_V29,                  .name = "demodsoft_V29"},
    {.id =  471, .api = &autotest_demodsoft_arb16opt,             .name = "demodsoft_arb16opt"},
    {.id =  472, .api = &autotest_demodsoft_arb32opt,             .name = "demodsoft_arb32opt"},
    {.id =  473, .api = &autotest_demodsoft_arb64opt,             .name = "demodsoft_arb64opt"},
    {.id =  474, .api = &autotest_demodsoft_arb128opt,            .name = "demodsoft_arb128opt"},
    {.id =  475, .api = &autotest_demodsoft_arb256opt,            .name = "demodsoft_arb256opt"},
    {.id =  476, .api = &autotest_demodsoft_arb64vt,              .name = "demodsoft_arb64vt"},
    {.id =  477, .api = &autotest_demodstats_psk2,                .name = "demodstats_psk2"},
    {.id =  478, .api = &autotest_demodstats_psk4,                .name = "demodstats_psk4"},
    {.id =  479, .api = &autotest_demodstats_psk8,                .name = "demodstats_psk8"},
    {.id =  480, .api = &autotest_demodstats_psk16,               .name = "demodstats_psk16"},
    {.id =  481, .api = &autotest_demodstats_psk32,               .name = "demodstats_psk32"},
    {.id =  482, .api = &autotest_demodstats_psk64,               .name = "demodstats_psk64"},
    {.id =  483, .api = &autotest_demodstats_psk128,              .name = "demodstats_psk128"},
    {.id =  484, .api = &autotest_demodstats_psk256,              .name = "demodstats_psk256"},
    {.id =  485, .api = &autotest_demodstats_dpsk2,               .name = "demodstats_dpsk2"},
    {.id =  486, .api = &autotest_demodstats_dpsk4,               .name = "demodstats_dpsk4"},
    {.id =  487, .api = &autotest_demodstats_dpsk8,               .name = "demodstats_dpsk8"},
    {.id =  488, .api = &autotest_demodstats_dpsk16,              .name = "demodstats_dpsk16"},
    {.id =  489, .api = &autotest_demodstats_dpsk32,              .name = "demodstats_dpsk32"},
    {.id =  490, .api = &autotest_demodstats_dpsk64,              .name = "demodstats_dpsk64"},
    {.id =  491, .api = &autotest_demodstats_dpsk128,             .name = "demodstats_dpsk128"},
    {.id =  492, .api = &autotest_demodstats_dpsk256,             .name = "demodstats_dpsk256"},
    {.id =  493, .api = &autotest_demodstats_ask2,                .name = "demodstats_ask2"},
    {.id =  494, .api = &autotest_demodstats_ask4,                .name = "demodstats_ask4"},
    {.id =  495, .api = &autotest_demodstats_ask8,                .name = "demodstats_ask8"},
    {.id =  496, .api = &autotest_demodstats_ask16,               .name = "demodstats_ask16"},
    {.id =  497, .api = &autotest_demodstats_ask32,               .name = "demodstats_ask32"},
    {.id =  498, .api = &autotest_demodstats_ask64,               .name = "demodstats_ask64"},
    {.id =  499, .api = &autotest_demodstats_ask128,              .name = "demodstats_ask128"},
    {.id =  500, .api = &autotest_demodstats_ask256,              .name = "demodstats_ask256"},
    {.id =  501, .api = &autotest_demodstats_qam4,                .name = "demodstats_qam4"},
    {.id =  502, .api = &autotest_demodstats_qam8,                .name = "demodstats_qam8"},
    {.id =  503, .api = &autotest_demodstats_qam16,               .name = "demodstats_qam16"},
    {.id =  504, .api = &autotest_demodstats_qam32,               .name = "demodstats_qam32"},
    {.id =  505, .api = &autotest_demodstats_qam64,               .name = "demodstats_qam64"},
    {.id =  506, .api = &autotest_demodstats_qam128,              .name = "demodstats_qam128"},
    {.id =  507, .api = &autotest_demodstats_qam256,              .name = "demodstats_qam256"},
    {.id =  508, .api = &autotest_demodstats_apsk4,               .name = "demodstats_apsk4"},
    {.id =  509, .api = &autotest_demodstats_apsk8,               .name = "demodstats_apsk8"},
    {.id =  510, .api = &autotest_demodstats_apsk16,              .name = "demodstats_apsk16"},
    {.id =  511, .api = &autotest_demodstats_apsk32,              .name = "demodstats_apsk32"},
    {.id =  512, .api = &autotest_demodstats_apsk64,              .name = "demodstats_apsk64"},
    {.id =  513, .api = &autotest_demodstats_apsk128,             .name = "demodstats_apsk128"},
    {.id =  514, .api = &autotest_demodstats_apsk256,             .name = "demodstats_apsk256"},
    {.id =  515, .api = &autotest_demodstats_bpsk,                .name = "demodstats_bpsk"},
    {.id =  516, .api = &autotest_demodstats_qpsk,                .name = "demodstats_qpsk"},
    {.id =  517, .api = &autotest_demodstats_ook,                 .name = "demodstats_ook"},
    {.id =  518, .api = &autotest_demodstats_sqam32,              .name = "demodstats_sqam32"},
    {.id =  519, .api = &autotest_demodstats_sqam128,             .name = "demodstats_sqam128"},
    {.id =  520, .api = &autotest_demodstats_V29,                 .name = "demodstats_V29"},
    {.id =  521, .api = &autotest_demodstats_arb16opt,            .name = "demodstats_arb16opt"},
    {.id =  522, .api = &autotest_demodstats_arb32opt,            .name = "demodstats_arb32opt"},
    {.id =  523, .api = &autotest_demodstats_arb64opt,            .name = "demodstats_arb64opt"},
    {.id =  524, .api = &autotest_demodstats_arb128opt,           .name = "demodstats_arb128opt"},
    {.id =  525, .api = &autotest_demodstats_arb256opt,           .name = "demodstats_arb256opt"},
    {.id =  526, .api = &autotest_demodstats_arb64vt,             .name = "demodstats_arb64vt"},
    {.id =  527, .api = &autotest_firpfbch2_crcf_n8,              .name = "firpfbch2_crcf_n8"},
    {.id =  528, .api = &autotest_firpfbch2_crcf_n16,             .name = "firpfbch2_crcf_n16"},
    {.id =  529, .api = &autotest_firpfbch2_crcf_n32,             .name = "firpfbch2_crcf_n32"},
    {.id =  530, .api = &autotest_firpfbch2_crcf_n64,             .name = "firpfbch2_crcf_n64"},
    {.id =  531, .api = &autotest_firpfbch_crcf_synthesis,        .name = "firpfbch_crcf_synthesis"},
    {.id =  532, .api = &autotest_firpfbch_crcf_analysis,         .name = "firpfbch_crcf_analysis"},
    {.id =  533, .api = &autotest_ofdmframesync_acquire_n64,      .name = "ofdmframesync_acquire_n64"},
    {.id =  534, .api = &autotest_ofdmframesync_acquire_n128,     .name = "ofdmframesync_acquire_n128"},
    {.id =  535, .api = &autotest_ofdmframesync_acquire_n256,     .name = "ofdmframesync_acquire_n256"},
    {.id =  536, .api = &autotest_ofdmframesync_acquire_n512,     .name = "ofdmframesync_acquire_n512"},
    {.id =  537, .api = &autotest_nco_crcf_frequency,             .name = "nco_crcf_frequency"},
    {.id =  538, .api = &autotest_vco_crcf_frequency,             .name = "vco_crcf_frequency"},
    {.id =  539, .api = &autotest_nco_crcf_phase,                 .name = "nco_crcf_phase"},
    {.id =  540, .api = &autotest_vco_crcf_phase,                 .name = "vco_crcf_phase"},
    {.id =  541, .api = &autotest_nco_basic,                      .name = "nco_basic"},
    {.id =  542, .api = &autotest_nco_mixing,                     .name = "nco_mixing"},
    {.id =  543, .api = &autotest_nco_block_mixing,               .name = "nco_block_mixing"},
    {.id =  544, .api = &autotest_vco_crcf_pll_phase,             .name = "vco_crcf_pll_phase"},
    {.id =  545, .api = &autotest_nco_crcf_pll_phase,             .name = "nco_crcf_pll_phase"},
    {.id =  546, .api = &autotest_nco_unwrap_phase,               .name = "nco_unwrap_phase"},
    {.id =  547, .api = &autotest_gradsearch_rosenbrock,          .name = "gradsearch_rosenbrock"},
    {.id =  548, .api = &autotest_gradsearch_maxutility,          .name = "gradsearch_maxutility"},
    {.id =  549, .api = &autotest_compand_float,                  .name = "compand_float"},
    {.id =  550, .api = &autotest_compand_cfloat,                 .name = "compand_cfloat"},
    {.id =  551, .api = &autotest_quantize_float_n8,              .name = "quantize_float_n8"},
    {.id =  552, .api = &autotest_scramble_n16,                   .name = "scramble_n16"},
    {.id =  553, .api = &autotest_scramble_n64,                   .name = "scramble_n64"},
    {.id =  554, .api = &autotest_scramble_n256,                  .name = "scramble_n256"},
    {.id =  555, .api = &autotest_scramble_n11,                   .name = "scramble_n11"},
    {.id =  556, .api = &autotest_scramble_n33,                   .name = "scramble_n33"},
    {.id =  557, .api = &autotest_scramble_n277,                  .name = "scramble_n277"},
    {.id =  558, .api = &autotest_scramble_soft_n16,              .name = "scramble_soft_n16"},
    {.id =  559, .api = &autotest_scramble_soft_n64,              .name = "scramble_soft_n64"},
    {.id =  560, .api = &autotest_scramble_soft_n256,             .name = "scramble_soft_n256"},
    {.id =  561, .api = &autotest_scramble_soft_n11,              .name = "scramble_soft_n11"},
    {.id =  562, .api = &autotest_scramble_soft_n33,              .name = "scramble_soft_n33"},
    {.id =  563, .api = &autotest_scramble_soft_n277,             .name = "scramble_soft_n277"},
    {.id =  564, .api = &autotest_bsequence_init,                 .name = "bsequence_init"},
    {.id =  565, .api = &autotest_bsequence_correlate,            .name = "bsequence_correlate"},
    {.id =  566, .api = &autotest_bsequence_add,                  .name = "bsequence_add"},
    {.id =  567, .api = &autotest_bsequence_mul,                  .name = "bsequence_mul"},
    {.id =  568, .api = &autotest_bsequence_accumulate,           .name = "bsequence_accumulate"},
    {.id =  569, .api = &autotest_complementary_code_n8,          .name = "complementary_code_n8"},
    {.id =  570, .api = &autotest_complementary_code_n16,         .name = "complementary_code_n16"},
    {.id =  571, .api = &autotest_complementary_code_n32,         .name = "complementary_code_n32"},
    {.id =  572, .api = &autotest_complementary_code_n64,         .name = "complementary_code_n64"},
    {.id =  573, .api = &autotest_complementary_code_n128,        .name = "complementary_code_n128"},
    {.id =  574, .api = &autotest_complementary_code_n256,        .name = "complementary_code_n256"},
    {.id =  575, .api = &autotest_complementary_code_n512,        .name = "complementary_code_n512"},
    {.id =  576, .api = &autotest_bsequence_init_msequence,       .name = "bsequence_init_msequence"},
    {.id =  577, .api = &autotest_msequence_m2,                   .name = "msequence_m2"},
    {.id =  578, .api = &autotest_msequence_m3,                   .name = "msequence_m3"},
    {.id =  579, .api = &autotest_msequence_m4,                   .name = "msequence_m4"},
    {.id =  580, .api = &autotest_msequence_m5,                   .name = "msequence_m5"},
    {.id =  581, .api = &autotest_msequence_m6,                   .name = "msequence_m6"},
    {.id =  582, .api = &autotest_msequence_m7,                   .name = "msequence_m7"},
    {.id =  583, .api = &autotest_msequence_m8,                   .name = "msequence_m8"},
    {.id =  584, .api = &autotest_msequence_m9,                   .name = "msequence_m9"},
    {.id =  585, .api = &autotest_msequence_m10,                  .name = "msequence_m10"},
    {.id =  586, .api = &autotest_msequence_m11,                  .name = "msequence_m11"},
    {.id =  587, .api = &autotest_msequence_m12,                  .name = "msequence_m12"},
    {.id =  588, .api = &autotest_lbshift,                        .name = "lbshift"},
    {.id =  589, .api = &autotest_rbshift,                        .name = "rbshift"},
    {.id =  590, .api = &autotest_lbcircshift,                    .name = "lbcircshift"},
    {.id =  591, .api = &autotest_rbcircshift,                    .name = "rbcircshift"},
    {.id =  592, .api = &autotest_count_ones,                     .name = "count_ones"},
    {.id =  593, .api = &autotest_count_ones_mod2,                .name = "count_ones_mod2"},
    {.id =  594, .api = &autotest_bdotprod,                       .name = "bdotprod"},
    {.id =  595, .api = &autotest_count_leading_zeros,            .name = "count_leading_zeros"},
    {.id =  596, .api = &autotest_msb_index,                      .name = "msb_index"},
    {.id =  597, .api = &autotest_pack_array,                     .name = "pack_array"},
    {.id =  598, .api = &autotest_unpack_array,                   .name = "unpack_array"},
    {.id =  599, .api = &autotest_repack_array,                   .name = "repack_array"},
    {.id =  600, .api = &autotest_pack_bytes_01,                  .name = "pack_bytes_01"},
    {.id =  601, .api = &autotest_unpack_bytes_01,                .name = "unpack_bytes_01"},
    {.id =  602, .api = &autotest_repack_bytes_01,                .name = "repack_bytes_01"},
    {.id =  603, .api = &autotest_repack_bytes_02,                .name = "repack_bytes_02"},
    {.id =  604, .api = &autotest_repack_bytes_03,                .name = "repack_bytes_03"},
    {.id =  605, .api = &autotest_repack_bytes_04_uneven,         .name = "repack_bytes_04_uneven"},
    {.id =  606, .api = &autotest_lshift,                         .name = "lshift"},
    {.id =  607, .api = &autotest_rshift,                         .name = "rshift"},
    {.id =  608, .api = &autotest_lcircshift,                     .name = "lcircshift"},
    {.id =  609, .api = &autotest_rcircshift,                     .name = "rcircshift"}
};

// array of packages
package_t packages[NUM_PACKAGES] = {
    {.id =    0, .index =    0, .num_scripts =    1, .name = "null"},
    {.id =    1, .index =    1, .num_scripts =    4, .name = "agc_crcf"},
    {.id =    2, .index =    5, .num_scripts =    1, .name = "cvsd"},
    {.id =    3, .index =    6, .num_scripts =    3, .name = "cbuffer"},
    {.id =    4, .index =    9, .num_scripts =    1, .name = "wdelay"},
    {.id =    5, .index =   10, .num_scripts =    1, .name = "window"},
    {.id =    6, .index =   11, .num_scripts =    8, .name = "dotprod_rrrf"},
    {.id =    7, .index =   19, .num_scripts =    3, .name = "dotprod_crcf"},
    {.id =    8, .index =   22, .num_scripts =    3, .name = "dotprod_cccf"},
    {.id =    9, .index =   25, .num_scripts =    6, .name = "sumsqf"},
    {.id =   10, .index =   31, .num_scripts =    6, .name = "sumsqcf"},
    {.id =   11, .index =   37, .num_scripts =    1, .name = "eqrls_rrrf"},
    {.id =   12, .index =   38, .num_scripts =    8, .name = "crc"},
    {.id =   13, .index =   46, .num_scripts =   26, .name = "fec"},
    {.id =   14, .index =   72, .num_scripts =   22, .name = "fec_soft"},
    {.id =   15, .index =   94, .num_scripts =    1, .name = "fec_golay2412"},
    {.id =   16, .index =   95, .num_scripts =    2, .name = "fec_hamming74"},
    {.id =   17, .index =   97, .num_scripts =    2, .name = "fec_hamming84"},
    {.id =   18, .index =   99, .num_scripts =    2, .name = "fec_hamming128"},
    {.id =   19, .index =  101, .num_scripts =    1, .name = "fec_hamming1511"},
    {.id =   20, .index =  102, .num_scripts =    1, .name = "fec_hamming3126"},
    {.id =   21, .index =  103, .num_scripts =    1, .name = "fec_reedsolomon"},
    {.id =   22, .index =  104, .num_scripts =    1, .name = "fec_rep3"},
    {.id =   23, .index =  105, .num_scripts =    1, .name = "fec_rep5"},
    {.id =   24, .index =  106, .num_scripts =    3, .name = "fec_secded2216"},
    {.id =   25, .index =  109, .num_scripts =    3, .name = "fec_secded3932"},
    {.id =   26, .index =  112, .num_scripts =    3, .name = "fec_secded7264"},
    {.id =   27, .index =  115, .num_scripts =    8, .name = "interleaver"},
    {.id =   28, .index =  123, .num_scripts =    3, .name = "packetizer"},
    {.id =   29, .index =  126, .num_scripts =    5, .name = "fft_small"},
    {.id =   30, .index =  131, .num_scripts =    6, .name = "fft_radix2"},
    {.id =   31, .index =  137, .num_scripts =   15, .name = "fft_composite"},
    {.id =   32, .index =  152, .num_scripts =    6, .name = "fft_prime"},
    {.id =   33, .index =  158, .num_scripts =   24, .name = "fft_r2r"},
    {.id =   34, .index =  182, .num_scripts =    2, .name = "fft_shift"},
    {.id =   35, .index =  184, .num_scripts =   12, .name = "fftfilt_xxxf"},
    {.id =   36, .index =  196, .num_scripts =    1, .name = "filter_crosscorr"},
    {.id =   37, .index =  197, .num_scripts =   12, .name = "firdecim_xxxf"},
    {.id =   38, .index =  209, .num_scripts =    3, .name = "firdes"},
    {.id =   39, .index =  212, .num_scripts =    2, .name = "firdespm"},
    {.id =   40, .index =  214, .num_scripts =   12, .name = "firfilt_xxxf"},
    {.id =   41, .index =  226, .num_scripts =    2, .name = "firhilb"},
    {.id =   42, .index =  228, .num_scripts =    2, .name = "firinterp"},
    {.id =   43, .index =  230, .num_scripts =    1, .name = "firpfb"},
    {.id =   44, .index =  231, .num_scripts =    4, .name = "groupdelay"},
    {.id =   45, .index =  235, .num_scripts =    6, .name = "iirdes"},
    {.id =   46, .index =  241, .num_scripts =    9, .name = "iirfilt_xxxf"},
    {.id =   47, .index =  250, .num_scripts =    2, .name = "iirfiltsos_rrrf"},
    {.id =   48, .index =  252, .num_scripts =    1, .name = "msresamp_crcf"},
    {.id =   49, .index =  253, .num_scripts =    1, .name = "resamp_crcf"},
    {.id =   50, .index =  254, .num_scripts =    2, .name = "resamp2_crcf"},
    {.id =   51, .index =  256, .num_scripts =    4, .name = "symsync_crcf"},
    {.id =   52, .index =  260, .num_scripts =    4, .name = "symsync_rrrf"},
    {.id =   53, .index =  264, .num_scripts =    1, .name = "bpacketsync"},
    {.id =   54, .index =  265, .num_scripts =    2, .name = "bsync"},
    {.id =   55, .index =  267, .num_scripts =   10, .name = "detector"},
    {.id =   56, .index =  277, .num_scripts =    1, .name = "flexframesync"},
    {.id =   57, .index =  278, .num_scripts =    1, .name = "framesync64"},
    {.id =   58, .index =  279, .num_scripts =   20, .name = "qdetector_cccf"},
    {.id =   59, .index =  299, .num_scripts =    8, .name = "qpacketmodem"},
    {.id =   60, .index =  307, .num_scripts =    5, .name = "qpilotsync"},
    {.id =   61, .index =  312, .num_scripts =    3, .name = "kbd"},
    {.id =   62, .index =  315, .num_scripts =    3, .name = "math"},
    {.id =   63, .index =  318, .num_scripts =    5, .name = "math_bessel"},
    {.id =   64, .index =  323, .num_scripts =    5, .name = "math_gamma"},
    {.id =   65, .index =  328, .num_scripts =    6, .name = "math_complex"},
    {.id =   66, .index =  334, .num_scripts =   10, .name = "polynomial"},
    {.id =   67, .index =  344, .num_scripts =   10, .name = "matrixcf"},
    {.id =   68, .index =  354, .num_scripts =   12, .name = "matrixf"},
    {.id =   69, .index =  366, .num_scripts =    4, .name = "smatrixb"},
    {.id =   70, .index =  370, .num_scripts =    2, .name = "smatrixf"},
    {.id =   71, .index =  372, .num_scripts =    2, .name = "smatrixi"},
    {.id =   72, .index =  374, .num_scripts =    3, .name = "freqmodem"},
    {.id =   73, .index =  377, .num_scripts =   50, .name = "modem"},
    {.id =   74, .index =  427, .num_scripts =   50, .name = "modem_demodsoft"},
    {.id =   75, .index =  477, .num_scripts =   50, .name = "modem_demodstats"},
    {.id =   76, .index =  527, .num_scripts =    4, .name = "firpfbch2_crcf"},
    {.id =   77, .index =  531, .num_scripts =    1, .name = "firpfbch_crcf_synthesizer"},
    {.id =   78, .index =  532, .num_scripts =    1, .name = "firpfbch_crcf_analyzer"},
    {.id =   79, .index =  533, .num_scripts =    4, .name = "ofdmframesync"},
    {.id =   80, .index =  537, .num_scripts =    2, .name = "nco_crcf_frequency"},
    {.id =   81, .index =  539, .num_scripts =    5, .name = "nco_crcf_phase"},
    {.id =   82, .index =  544, .num_scripts =    2, .name = "nco_crcf_pll"},
    {.id =   83, .index =  546, .num_scripts =    1, .name = "unwrap_phase"},
    {.id =   84, .index =  547, .num_scripts =    2, .name = "gradsearch"},
    {.id =   85, .index =  549, .num_scripts =    2, .name = "compand"},
    {.id =   86, .index =  551, .num_scripts =    1, .name = "quantize"},
    {.id =   87, .index =  552, .num_scripts =   12, .name = "scramble"},
    {.id =   88, .index =  564, .num_scripts =    5, .name = "bsequence"},
    {.id =   89, .index =  569, .num_scripts =    7, .name = "complementary_codes"},
    {.id =   90, .index =  576, .num_scripts =   12, .name = "msequence"},
    {.id =   91, .index =  588, .num_scripts =    4, .name = "bshift_array"},
    {.id =   92, .index =  592, .num_scripts =    5, .name = "count_bits"},
    {.id =   93, .index =  597, .num_scripts =    9, .name = "pack_bytes"},
    {.id =   94, .index =  606, .num_scripts =    4, .name = "shift_array"}
};

#endif // __LIQUID_AUTOSCRIPT_INCLUDE_H__

