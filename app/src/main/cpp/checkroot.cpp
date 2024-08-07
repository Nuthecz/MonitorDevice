#include <sys/syscall.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "utils/basic.h"

bool NativeDetected(const char *path, bool usesyscall) {
    if (usesyscall) {//使用系统调用的方式，查找文件是否存在
        long ret =  syscall(SYS_faccessat, AT_FDCWD, path, 0);
        if (ret == 0) {
            return true;
        } else {
            return false;
        }
    } else {
        struct stat buf{};
        //使用linux内核提供的方法
        if (access(path, F_OK) == 0) {
            return true;
        };
        if (stat(path, &buf) == 0) {//linux 文件属性结构体
            return true;
        }
        if (fstat(open(path, O_PATH), &buf) == 0) {
            return true;
        }
    };
    return false;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_monitordevice_CheckRootActivity_checkUseso(
        JNIEnv *env,
        jobject /* this */,
        jobjectArray targetPaths,
        jobjectArray fileNames) {

    int pathCount = env->GetArrayLength(targetPaths);
    int nameCount = env->GetArrayLength(fileNames);

    // 遍历targetPaths数组
    for (int pathIndex = 0; pathIndex < pathCount; pathIndex++) {
        auto j_path = env->GetObjectArrayElement(targetPaths, pathIndex);
        const char* path = env->GetStringUTFChars(static_cast<jstring>(j_path), nullptr);

        // 遍历fileNames数组
        for (int nameIndex = 0; nameIndex < nameCount; nameIndex++) {
            auto j_name = env->GetObjectArrayElement(fileNames, nameIndex);
            const char* name = env->GetStringUTFChars(static_cast<jstring>(j_name), nullptr);

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




