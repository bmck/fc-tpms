// $Id$
// $(c)$

//
// Created by Bill McKinnon on 10/8/15.
//

#ifndef SDR_FTP_SERVICE_H
#define SDR_FTP_SERVICE_H


#include <stdio.h>
#include <stdlib.h>

#include <jni.h>
JNIEXPORT jstring JNICALL
        Java_com_fleetcents_remote_1tpms_fleetcentstpmsremoteclient_SdrFtpService_analyzeFile
        (JNIEnv *, jclass, jstring);

#endif //REMOTE_TPMS_ANALYZE_TPMS2_BRIDGE_H
