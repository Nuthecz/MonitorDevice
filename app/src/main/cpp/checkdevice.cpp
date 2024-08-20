#include "utils/basic.h"
#include <sys/statfs.h>
#include <cstdlib>


extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_checkdevice_DeviceCheck_getHardDiskInfor(JNIEnv *env, jobject thiz) {
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
JNIEXPORT jstring JNICALL
Java_com_example_checkdevice_DeviceCheck_getKernelInfor(JNIEnv *env, jobject thiz) {
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