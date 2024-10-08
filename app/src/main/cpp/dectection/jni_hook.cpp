//
// Created by NU on 2024/10/8.
//

#include "../include/hookDetection.h"

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_nuthecz_checkhook_HookCheck_checkHook(JNIEnv *env, jobject thiz) {
    std::string result;
    // 检测inlineHook，它比 frida 处的 inlineHook 更为强力
    // 这里只要曾经被 hook，那么它就会一直显示存在 hook 行为，除非手机重启
    if (segmentHookStatus()) {
        result = "There is inlineHook behavior";
    }

    if (openHookStatus()) {
        if (!result.empty()) result += "\n";
        LOGI("libc.so -> Open Modified");
        result += "libc.so -> Open Modified";
    }

    if (prettyMethodHookStatus()) {
        if (!result.empty()) result += "\n";
        LOGI("PrettyMethodHook Detected");
        result += "PrettyMethodHook Detected";
    }

    if (result.empty()) {
        result = "No Hook Detected";
    }
    return env->NewStringUTF(result.c_str());
}

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_nuthecz_checkhook_FridaCheck_checkFrida(
        JNIEnv *env,
        jobject /* this */) {
    std::string result;
    if (frida_check_status()) {
        LOGI("Detected Frida!!!(So)");
        LOGI("Status Detected");
        result += "Status Detected";
    }

    if (frida_check_maps()) {
        if (!result.empty()) result += "\n";
        LOGI("Detected Frida!!!(So)");
        LOGI("Maps Detected");
        result += "Maps Detected";
    }

    if (frida_check_maps_self()) {
        if (!result.empty()) result += "\n";
        LOGI("Detected Frida!!!(So)");
        LOGI("Maps Self-fulfillment Detected");
        result += "Maps Self-fulfillment Detected";
    }

    if (result.empty()) {
        result = "No Frida Detected";
    }
    return env->NewStringUTF(result.c_str());
}

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_nuthecz_checkhook_XposedCheck_soCheckXposed(JNIEnv *env, jobject thiz) {
    std::string result;

    // proc/pid/maps 检测
    std::string str = "/proc/" + std::to_string(getpid()) + "/maps";
    char *file = const_cast<char *>(str.c_str());
    if (xposed_check_maps(file)) {
        LOGI("Detected Xposed!!!(So)");
        LOGI("Maps Detected");
        result += "Maps Detected";
    }
    // stack 检测
    if (xposed_check_stack(env)) {
        if (!result.empty()) result += "\n";
        LOGI("Detected Xposed!!!(So)");
        LOGI("StackTrace Detected");
        result += "StackTrace Detected(So)";
    }
    return env->NewStringUTF(result.c_str());
}
