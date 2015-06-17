// $Id$
// $(c)$

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <pthread.h>
#include <jni.h>

#define MAX_CHARS_IN_CLI_SEND_STRF (512)

// static JavaVM *jvm;
// static int javaversion;
// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

JNIEXPORT void JNICALL Java_com_fleetcents_generic_1tpms_convertAndGetPkt
(JNIEnv * env, jclass class) {
  /* Get a handle on our calling NativeActivity class */
  jclass activityClass = (*env)->FindClass(env, "android/app/Activity/MainActivity");

  /* Get path to cache dir (/data/data/org.myapp/cache) */
  jmethodID getCacheDir = (*env)->GetMethodID(env, activityClass, "getCacheDir", "()Ljava/io/File;");
  jobject file = (*env)->CallObjectMethod(env, activityClass, getCacheDir);
  jclass fileClass = (*env)->FindClass(env, "java/io/File");
  jmethodID getAbsolutePath = (*env)->GetMethodID(env, fileClass, "getAbsolutePath", "()Ljava/lang/String;");
  jstring jpath = (jstring)(*env)->CallObjectMethod(env, file, getAbsolutePath);
  const char* app_dir = (*env)->GetStringUTFChars(env, jpath, NULL);

  char binfilename[300];
  strcpy(binfilename, app_dir);
  strcat(binfilename, "/");
  strcat(binfilename, "fc.bin");

  char cplxfilename[300];
  strncpy(cplxfilename, binfilename, strlen(binfilename)-3);
  strcat(cplxfilename, "cplx");

  iqfile_to_cplxfile(binfilename, cplxfilename);
  get_pkt(cplxfilename);
}

JNIEXPORT double JNICALL Java_com_fleetcents_generic_1tpms_getPsi
(JNIEnv * env, jclass class) {
  return get_pressure_psi();
}

JNIEXPORT double JNICALL Java_com_fleetcents_generic_1tpms_getKpa
(JNIEnv * env, jclass class) {
  return get_pressure_kpa();
}

JNIEXPORT long JNICALL Java_com_fleetcents_generic_1tpms_getTempF
(JNIEnv * env, jclass class) {
  return get_temp_f();
}

JNIEXPORT long JNICALL Java_com_fleetcents_generic_1tpms_getTempC
(JNIEnv * env, jclass class) {
  return get_temp_c();
}

JNIEXPORT unsigned long JNICALL Java_com_fleetcents_generic_1tpms_getDecAddr
(JNIEnv * env, jclass class) {
  return get_dec_address_val();
}

JNIEXPORT jstring JNICALL Java_com_fleetcents_generic_1tpms_getHexAddr
(JNIEnv * env, jclass class) {

  char *retVal; get_hex_address_str(retVal);
  jstring strng; strng = ((*env)->NewStringUTF(env, retVal));
  free(retVal);
  return strng;
}

JNIEXPORT jstring JNICALL Java_com_fleetcents_generic_1tpms_getFullUrl
(JNIEnv * env, jclass class) {

  char *retVal; get_url(retVal);
  jstring strng; strng = ((*env)->NewStringUTF(env, retVal));
  free(retVal);
  return strng;
}

