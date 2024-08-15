#include "utils/basic.h"
#include <string>
#include <vector>

bool check_maps(char *path) {
    bool flag = false;
    FILE *fp = fopen(path, "r");
    if (fp == nullptr) {
        return false;
    }
    char *line = nullptr;
    size_t len = 0;
    while (getline(&line, &len, fp) != -1) {
        if (strstr(line, "edxp") || strstr(line, "magisk") || strstr(line, "zygisk") ||
            strstr(line, "Hook") || strstr(line, "lsp")) {
            flag = true;
            break;
        }
    }
    fclose(fp);
    free(line);
    return flag;
}

bool check_stack(JNIEnv *env) {
    // 在 Native 层反射调用 getStackTrace 检测
    jclass threadClass = env->FindClass("java/lang/Thread");
    jmethodID currentThread = env->GetStaticMethodID(threadClass, "currentThread",
                                                     "()Ljava/lang/Thread;");
    jobject thread = env->CallStaticObjectMethod(threadClass, currentThread);
    jmethodID getStackTrace = env->GetMethodID(threadClass, "getStackTrace",
                                               "()[Ljava/lang/StackTraceElement;");
    auto stacktrace = (jobjectArray) env->CallObjectMethod(thread, getStackTrace);
    int length = env->GetArrayLength(stacktrace);
    jclass stackTraceElementClass = env->FindClass("java/lang/StackTraceElement");
    jmethodID getClass = env->GetMethodID(stackTraceElementClass, "getClassName",
                                          "()Ljava/lang/String;");

    for (int i = 0; i < length; i++) {
        jobject straceElement = env->GetObjectArrayElement(stacktrace, i);
        auto classname = (jstring) env->CallObjectMethod(straceElement, getClass);
        const char *name = env->GetStringUTFChars(classname, 0);

        if (strstr(name, "xposed") || strstr(name, "EdHooker")) {
            return true;
        }

        // 当在 JNI 中获得一个 Java 对象的引用，Java 虚拟机需要知道这个引用何时不再被使用，这样垃圾回收器就可以回收它。
        env->ReleaseStringUTFChars(classname, name); // Release string chars
        env->DeleteLocalRef(classname); // Release the local reference
        env->DeleteLocalRef(straceElement);
    }
    return false;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_checkhook_XposedCheck_soCheckXposed(JNIEnv *env, jobject thiz) {
    std::string result;

    // proc/pid/maps 检测
    std::string str = "/proc/" + std::to_string(getpid()) + "/maps";
    char *file = const_cast<char *>(str.c_str());
    if (check_maps(file)) {
        LOGI("Detected Xposed!!!(So)");
        LOGI("Maps Detected");
        result += "Maps Detected";
    }
    // stack 检测
    if (check_stack(env)) {
        if (!result.empty()) result += "\n";
        LOGI("Detected Xposed!!!(So)");
        LOGI("StackTrace Detected");
        result += "StackTrace Detected(So)";
    }
    return env->NewStringUTF(result.c_str());
}
