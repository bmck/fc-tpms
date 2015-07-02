// $Id$
// $(c)$

// Taken from https://gist.github.com/Nimrodda/2bd63e118d36276248e7

#ifndef _FC_LOG_H
#define _FC_LOG_H

#ifndef __LOG_MACROS
#define __LOG_MACROS
#endif


// #ifdef FCDBG
//   #define LOGV(...) ((void)fprintf(fcdebug, __VA_ARGS__))
//   #define LOGD(...) ((void)fprintf(fcdebug, __VA_ARGS__))
//   #define LOGI(...) ((void)fprintf(fcdebug, __VA_ARGS__))
//   #define LOGW(...) ((void)fprintf(fcdebug, __VA_ARGS__))
//   #define LOGE(...) ((void)fprintf(fcdebug, __VA_ARGS__))
// #else
  #define LOG_TAG "FleetCents"
  #include <android/log.h>
  #define LOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, "FleetCents", __VA_ARGS__))
  #define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "FleetCents", __VA_ARGS__))
  #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "FleetCents", __VA_ARGS__))
  #define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "FleetCents", __VA_ARGS__))
  #define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "FleetCents", __VA_ARGS__))
// #endif

#endif