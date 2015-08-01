
#include <jni.h>
#include "liquid_analysis.h"
#include "log_macros.h"

// JNI calls

JNIEXPORT int JNICALL Java_com_fleetcents_generic_1tpms_MainActivity_pktFound(JNIEnv * env, jclass klass) {
  return get_success();
}

JNIEXPORT void JNICALL Java_com_fleetcents_generic_1tpms_MainActivity_setCacheDir(JNIEnv * env, jclass klass, jstring str) {

   const char *nativeString = (*env)->GetStringUTFChars(env, str, 0);
   // LOGI("(%s:%d) nativeString = |%s|\n", __FILE__, __LINE__, nativeString);
   char *app_dir; app_dir = get_app_dir();
   strncpy(app_dir, nativeString, 200);
   // LOGI("app_dir = |%s|\n", app_dir);
   (*env)->ReleaseStringUTFChars(env, str, nativeString);
   // LOGI("app_dir = |%s|\n", app_dir);
}

#ifdef __FALSE__

JNIEXPORT void JNICALL Java_com_fleetcents_generic_1tpms_MainActivity_convertAndGetPkt(JNIEnv * env, jclass klass) {

  find_pkt_in_binfile();
}

#endif

JNIEXPORT double JNICALL Java_com_fleetcents_generic_1tpms_MainActivity_getPsi(JNIEnv * env, jclass klass) {
  return get_pressure_psi();
}

JNIEXPORT double JNICALL Java_com_fleetcents_generic_1tpms_MainActivity_getKpa(JNIEnv * env, jclass klass) {
  return get_pressure_kpa();
}

JNIEXPORT long JNICALL Java_com_fleetcents_generic_1tpms_MainActivity_getTempF(JNIEnv * env, jclass klass) {
  return get_temp_f();
}

JNIEXPORT long JNICALL Java_com_fleetcents_generic_1tpms_MainActivity_getTempC(JNIEnv * env, jclass klass) {
  long c; c = get_temp_c();
  LOGI("(%s:%d) c = %ld\n", __FILE__, __LINE__, c);
  return c;
}

JNIEXPORT unsigned long JNICALL Java_com_fleetcents_generic_1tpms_MainActivity_getDecAddr(JNIEnv * env, jclass klass) {
  return get_dec_address_val();
}

JNIEXPORT jstring JNICALL Java_com_fleetcents_generic_1tpms_MainActivity_getHexAddr(JNIEnv * env, jclass klass) {

  char *retVal; retVal = NULL; retVal = get_hex_address_str(retVal);
  LOGI("(%s:%d) retVal = %s\n", __FILE__, __LINE__, retVal);
  jstring strng; strng = ((*env)->NewStringUTF(env, retVal));
  free(retVal);
  return strng;
}

JNIEXPORT jstring JNICALL Java_com_fleetcents_generic_1tpms_MainActivity_getFullUrl(JNIEnv * env, jclass klass) {

  char *retVal; retVal = NULL; retVal = get_url(retVal);
  LOGI("(%s:%d) retVal = %s\n", __FILE__, __LINE__, retVal);
  jstring strng; strng = ((*env)->NewStringUTF(env, retVal));
  free(retVal);
  return strng;
}
