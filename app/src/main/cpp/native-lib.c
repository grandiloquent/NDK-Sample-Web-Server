#include <jni.h>


JNIEXPORT jint JNICALL
Java_euphoria_psycho_server_MainActivity_startServer(JNIEnv *env, jclass type, jstring host_,
                                                     jint port, jstring staticDirectory_) {
    const char *host = (*env)->GetStringUTFChars(env, host_, 0);
    const char *staticDirectory = (*env)->GetStringUTFChars(env, staticDirectory_, 0);

    (*env)->ReleaseStringUTFChars(env, host_, host);
    (*env)->ReleaseStringUTFChars(env, staticDirectory_, staticDirectory);

    return 0;
}