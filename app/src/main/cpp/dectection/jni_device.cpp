//
// Created by NU on 2024/10/8.
//

// 集成 java 与 native 通信
#include "../include/deviceDetection.h"

// root 检测
extern "C"
JNIEXPORT jboolean

JNICALL
Java_com_nuthecz_monitordevice_CheckRootActivity_checkUseso(
        JNIEnv *env,
        jobject /* this */,
        jobjectArray targetPaths,
        jobjectArray fileNames) {

    int pathCount = env->GetArrayLength(targetPaths);
    int nameCount = env->GetArrayLength(fileNames);

    // 遍历targetPaths数组
    for (int pathIndex = 0; pathIndex < pathCount; pathIndex++) {
        auto j_path = env->GetObjectArrayElement(targetPaths, pathIndex);
        const char *path = env->GetStringUTFChars(static_cast<jstring>(j_path), nullptr);

        // 遍历fileNames数组
        for (int nameIndex = 0; nameIndex < nameCount; nameIndex++) {
            auto j_name = env->GetObjectArrayElement(fileNames, nameIndex);
            const char *name = env->GetStringUTFChars(static_cast<jstring>(j_name), nullptr);

            // 组合路径
            char fullPath[PATH_MAX + 1];
            snprintf(fullPath, sizeof(fullPath), "%s%s", path, name);

            // 检测文件名对应的完整路径
            bool ret = NativeDetected(fullPath, true);
            if (ret) {
                LOGI("Detected Root!!!(So)");
                LOGI("Illegal binary %s exist", fullPath);
                return true;
            }
        }
    }
    return false;
}

// device 检测
extern "C"
JNIEXPORT jstring

JNICALL
Java_com_nuthecz_checkdevice_DeviceCheck_getHardDiskInfor(JNIEnv *env, jobject thiz) {
    // 获取 StatFs 类
    jclass statFsClass = env->FindClass("android/os/StatFs");
    jmethodID id = env->GetMethodID(statFsClass, "<init>", "(Ljava/lang/String;)V");
    jobject statFsObj = env->NewObject(statFsClass, id, env->NewStringUTF("/storage/emulated/0"));

    // 获取 total bytes
    jlong totalBytes = env->CallLongMethod(statFsObj,
                                           env->GetMethodID(statFsClass, "getTotalBytes", "()J"));

    // 使用 statfs64 获取更多信息
    struct statfs64 buf{};
    if (statfs64("/storage/emulated/0", &buf) == -1) {
        LOGE("statfs64 System Information Failed: %s", strerror(errno));
        return env->NewStringUTF("Error: Failed to get filesystem information.");
    }

    char info[4096];
    snprintf(info, sizeof(info),
             "Total Bytes: 0x%lx\n"
             "Filesystem Type: 0x%lx\n"
             "Block Size: 0x%lx\n"
             "Total Blocks: 0x%lx\n"
             "Free Blocks: 0x%lx\n"
             "Available Blocks: 0x%lx\n"
             "Total Inodes: 0x%lx\n"
             "Free Inodes: 0x%lx\n"
             "Filesystem ID: 0x%llx\n"
             "Maximum Filename Length: 0x%lx\n",
             totalBytes, // Total Bytes
             buf.f_type, // Filesystem Type
             buf.f_bsize, // Block Size
             buf.f_blocks, // Total Blocks
             buf.f_bfree, // Free Blocks
             buf.f_bavail, // Available Blocks
             buf.f_files, // Total Inodes
             buf.f_ffree, // Free Inodes
             ((unsigned long long) buf.f_fsid.__val[0] << 32) |
             buf.f_fsid.__val[1], // Filesystem ID
             buf.f_namelen); // Maximum Filename Length

    LOGI("Disk Info: %s", info);
    return env->NewStringUTF(info);
}

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_nuthecz_checkdevice_DeviceCheck_getKernelInfor(JNIEnv *env, jobject thiz) {
    char buffer[1024];
    FILE *fp = popen("uname -a", "r"); // “r” 则文件指针连接到 command 的标准输出，“w” 则文件指针连接到 command 的标准输入
    if (fp != nullptr) {
        while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
            LOGI("System Info: %s", buffer);
        };
    }
    pclose(fp);
    return env->NewStringUTF(buffer);
}

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_nuthecz_checkenv_EmulatorCheck_checkEmu(JNIEnv *env, jobject thiz) {
    std::string result;
    if (simulator_files_check()) {
        LOGI("Detected Emu!!!(So)");
        LOGI("Simulator Files Detected");
        result += "Simulator Files Detected";
    }
    if (!result.empty()) {
        result += "\n";
    }
    result += check_arch_by_seccomp(env);
    return env->NewStringUTF(result.c_str());
}


extern "C"
JNIEXPORT jstring

JNICALL
Java_com_nuthecz_checkenv_SandboxCheck_checkSandboxProcess(JNIEnv *env, jobject thiz) {
    // 通过 ps -ef 检测是否存在其他进程
    char buffer[1024];
    FILE *fp = popen("ps -ef", "r"); // “r” 则文件指针连接到 command 的标准输出，“w” 则文件指针连接到 command 的标准输入
    int size = 0;
    if (fp != nullptr) {
        while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
            // 不包含 ++
            if (!strstr(buffer, "monitor")) {
                size++;
            }
            // 当前进程输出以下信息，若是存在多余信息，表示有其他进程正在监控，因此不包含的个数大于2则认为存在沙箱
            // ps -ef: UID            PID  PPID C STIME TTY          TIME CMD
            // ps -ef: u0_a149      26726   801 36 16:37:25 ?    00:00:02 com.example.monitordevice
            // ps -ef: u0_a149      26764  26726 0 16:37:30 ?     00:00:00 ps -ef
        };
    }
    pclose(fp);
    std::string result;
    if (size > 2) {
        LOGI("Detected Sandbox!!!(retval)");
        LOGI("Sandbox Detected");
        result += "Sandbox Process Detected";
    }
    return env->NewStringUTF(result.c_str());
}


