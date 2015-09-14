/*
 * rtl_tcp_andro is an Android port of the famous rtl_tcp driver for
 * RTL2832U based USB DVB-T dongles. It does not require root.
 * Copyright (C) 2012 by Martin Marinov <martintzvetomirov@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RTL_SDR_ANDRO_H_
#define RTL_SDR_ANDRO_H_

#include "log_macros.h"

void rtlsdr_main(int usbfd, const char * uspfs_path_input, int argc, char **argv);
void rtlsdr_fc_close();
int rtlsdr_isrunning();

//void aprintf( const char* format , ... );
//void aprintf_stderr( const char* format , ... );
void announce_exceptioncode( const int exception_code );
void announce_success( );
void thread_detach();

extern void save_sample_rate(uint32_t);
extern int analyze_input(uint8_t *, uint32_t, uint32_t);
extern int analyze_file(char *);
extern int analyze_iq_pairs(unsigned char *buf, uint32_t buf_length);


#endif
