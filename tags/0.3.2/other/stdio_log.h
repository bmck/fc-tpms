// $Id$
// $(c)$

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

#include <stdio.h>

#define LOGV(...) printf("FleetCents VERBOSE -- "); printf(__VA_ARGS__); printf("\n");
#define LOGD(...) printf("FleetCents DEBUG -- "); printf(__VA_ARGS__); printf("\n");
#define LOGI(...) printf("FleetCents INFO -- "); printf(__VA_ARGS__); printf("\n");
#define LOGW(...) printf("FleetCents WARNING -- "); printf(__VA_ARGS__); printf("\n");
#define LOGE(...) printf("FleetCents  *** ERROR ***  -- "); printf(__VA_ARGS__); printf("\n");

