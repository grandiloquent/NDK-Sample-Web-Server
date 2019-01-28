#include <jni.h>
JNIEXPORT jstring JNICALL
Java_euphoria_psycho_server_MainActivity_startServer(JNIEnv *env, jclass type,
                                                     jstring staticDirectory_) {
    const char *staticDirectory = (*env)->GetStringUTFChars(env, staticDirectory_, 0);


    (*env)->ReleaseStringUTFChars(env, staticDirectory_, staticDirectory);

    return (*env)->NewStringUTF(env, "");
}