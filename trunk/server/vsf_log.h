// $Id$
// $(c)$


#ifndef __VSF_LOG_H
#define __VSF_LOG_H

#include "generic_log.h"
#include "vsftpd-3.0.3/logging.h"
#include "vsftpd-3.0.3/str.h"

void initialize_vsftpd_session(struct vsf_session* p_sess);
void logit(char *str);

#endif