// $Id$
// $(c)$


#ifndef __FILE_LOG_H
#define __FILE_LOG_H

#include <stdio.h>
#include <stdlib.h>

#define INIT_LOGGING FILE* logfp = NULL;
#define LOGFN "/tmp/fleet_analysis.log"
#define LOGFILE (logfp ? logfp : (logfp=fopen(LOGFN, "w+")))

#ifdef LOGV
#undef LOGV
#undef LOGD
#undef LOGI
#undef LOGW
#undef LOGE
#endif

#define LOGV(...) fprintf(LOGFILE, "FleetCents VERBOSE -- "); fprintf(LOGFILE, __VA_ARGS__);
#define LOGD(...) fprintf(LOGFILE, "FleetCents DEBUG -- "); fprintf(LOGFILE, __VA_ARGS__);
#define LOGI(...) fprintf(LOGFILE, "FleetCents INFO -- "); fprintf(LOGFILE, __VA_ARGS__);
#define LOGW(...) fprintf(LOGFILE, "FleetCents WARNING -- "); fprintf(LOGFILE, __VA_ARGS__);
#define LOGE(...) fprintf(LOGFILE, "FleetCents  *** ERROR ***  -- "); fprintf(LOGFILE, __VA_ARGS__);

#endif