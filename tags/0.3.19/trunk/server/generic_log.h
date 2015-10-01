// $Id$
// $(c)$


#ifndef __GENERIC_LOG_H
#define __GENERIC_LOG_H

#ifndef INIT_LOGGING
#define INIT_LOGGING
#endif

#ifdef LOGV
#undef LOGV
#undef LOGD
#undef LOGI
#undef LOGW
#undef LOGE
#endif

#define LOG(...) { char str[200]; sprintf(str, __VA_ARGS__); logit(str); }

#define LOGV(...) LOG(__VA_ARGS__)
#define LOGD(...) LOG(__VA_ARGS__)
#define LOGI(...) LOG(__VA_ARGS__)
#define LOGW(...) LOG(__VA_ARGS__)
#define LOGE(...) LOG(__VA_ARGS__)

#endif