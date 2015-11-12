// $Id$
// $(c)$

//
// Created by Bill McKinnon on 10/8/15.
//

#include "SdrFtpService.h"
#include "analyze_tpms2.h"

JNIEXPORT jstring JNICALL Java_com_fleetcents_remote_1tpms_fleetcentstpmsremoteclient_SdrFtpService_analyzeFile(JNIEnv* env, jclass class, jstring src_filename) {
    const char *src_fn = (*env)->GetStringUTFChars(env, src_filename, 0);

//	LOGI("Just before file analyzed");

    int analyzed_ok = analyze_file((char *) src_fn);
    char* returned_string = malloc(250 * sizeof(char));

//	LOGI("Just after file analyzed");

    if (analyzed_ok == 1) {
//		LOGI("File analyzed ok");

        char *hexaddr = NULL; hexaddr = get_hex_address_str(hexaddr);
//		LOGI("Got address");

        long tempc = get_temp_c();
//		LOGI("Got temp and ...");

        double press_kpa = get_pressure_kpa();
//		LOGI("Got pressure");

        sprintf(returned_string, "226 %s,%ld,%lf,", hexaddr, tempc, press_kpa);
    }

    else {
        strcpy(returned_string, "226 ,,,");
    }

    (*env)->ReleaseStringUTFChars(env, src_filename, src_fn);
    jstring jrtnd_string = (*env)->NewStringUTF(env, returned_string);

    free(returned_string);
    return jrtnd_string;
}