// $Id$
// $(c)$

#ifndef __STDIO_LOG_H
#define __STDIO_LOG_H

#ifndef __LOG_MACROS
#define __LOG_MACROS
#endif

#define LOGV(...) printf("FleetCents VERBOSE -- "); printf(__VA_ARGS__); printf("\n");
#define LOGD(...) printf("FleetCents DEBUG -- "); printf(__VA_ARGS__); printf("\n");
#define LOGI(...) printf("FleetCents INFO -- "); printf(__VA_ARGS__); printf("\n");
#define LOGW(...) printf("FleetCents WARNING -- "); printf(__VA_ARGS__); printf("\n");
#define LOGE(...) printf("FleetCents  *** ERROR ***  -- "); printf(__VA_ARGS__); printf("\n");

#endif