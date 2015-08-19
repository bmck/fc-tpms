// $Id$
// $(c)$

#include <stdlib.h>
#include "vsf_log.h"

static struct vsf_session* curr_sess = NULL;

void initialize_vsftpd_session(struct vsf_session* p_sess) {
  curr_sess = p_sess;
}

void logit(char *str) {
      struct mystr debugstr = INIT_MYSTR;
      str_empty(&debugstr);
      str_alloc_text(&debugstr, (const char *)str);
      vsf_log_line(curr_sess, kVSFLogEntryDebug, &debugstr);
}

