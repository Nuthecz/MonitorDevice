#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring
JNICALL
Java_com_example_monitordevice_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "This is a entry of MonitorDevice";
    return env->NewStringUTF(hello.c_str());
}