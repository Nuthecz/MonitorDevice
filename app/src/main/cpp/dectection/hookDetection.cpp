//
// Created by NU on 2024/10/8.
//

#include "../include/hookDetection.h"

// crc32 计算
uint32_t calculateCRC32(const unsigned char *data, size_t length) {
    uint32_t crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, data, length);
    return crc;
}

bool openHookStatus() {
    bool isHook = false;
    // 获取 libc open_offset
    SandHook::ElfImg libc("libc.so");
    void *open_addr = libc.getSymbAddress("open");
    uintptr_t open_offset = (uintptr_t) open_addr - (uintptr_t) libc.getBase();
    LOGI("open offset: 0x%x", open_offset);

    // 根据 open_offset 读取本地 libc open 的前16字节进行 CRC32 计算
    int fd = open(libc.name().c_str(), O_RDONLY);
    lseek(fd, open_offset, 0);
    char buf[16];
    read(fd, buf, 16);
    uintptr_t local_crc32_open_value = calculateCRC32(
            reinterpret_cast<const unsigned char *>(buf), 16);

    // 读取内存中的 open 前16字节进行 CRC32 计算
    uintptr_t mem_crc32_open_value = calculateCRC32(
            reinterpret_cast<const unsigned char *>(open_addr), 16);

    LOGI("local open crc32: 0x%x, mem open crc32: 0x%x", local_crc32_open_value,
         mem_crc32_open_value);
    if (local_crc32_open_value != mem_crc32_open_value) {
        isHook = true;
//        LOGE("open hook detected");
    }
    close(fd);
    return isHook;
}

bool segmentHookStatus() {
    bool isHook = false;
    // 获取 libc text_offset
    SandHook::ElfImg libc("libc.so");
    auto text_info = libc.getTextSectionInfo();
    LOGI("text info: 0x%x, 0x%x", text_info.first, text_info.second);

    // 根据 text_offset 读取本地 libc text 进行 CRC32 计算
    int fd = open(libc.name().c_str(), O_RDONLY);
    lseek(fd, text_info.first, 0);
    char buf[text_info.second];
    read(fd, buf, text_info.second);
    uintptr_t local_crc32_text_value = calculateCRC32(
            reinterpret_cast<const unsigned char *>(buf), text_info.second);

    // 读取内存中的 text 进行 CRC32 计算
    uintptr_t text_addr = (uintptr_t) libc.getBase() + text_info.first;
    uintptr_t mem_crc32_text_value = calculateCRC32(
            reinterpret_cast<const unsigned char *>(text_addr), text_info.second);

    LOGI("local text crc32: 0x%x, mem text crc32 0x%x", local_crc32_text_value,
         mem_crc32_text_value);

    if (local_crc32_text_value != mem_crc32_text_value) {
        isHook = true;
//        LOGE("text hook detected");
    }

    // 获取 libc plt_offset
    auto plt_info = libc.getPltSectionInfo();
    LOGI("plt info: 0x%x, 0x%x", plt_info.first, plt_info.second);

    // 根据 plt_offset 读取本地 libc plt 进行 CRC32 计算
    lseek(fd, plt_info.first, 0);
    char buf2[plt_info.second];
    read(fd, buf2, plt_info.second);
    uintptr_t local_crc32_plt_value = calculateCRC32(
            reinterpret_cast<const unsigned char *>(buf2), plt_info.second);

    // 读取内存中的 plt 进行 CRC32 计算
    uintptr_t plt_addr = (uintptr_t) libc.getBase() + plt_info.first;
    uintptr_t mem_crc32_plt_value = calculateCRC32(
            reinterpret_cast<const unsigned char *>(plt_addr), plt_info.second);

    LOGI("local plt crc32: 0x%x, mem plt crc32 0x%x", local_crc32_plt_value, mem_crc32_plt_value);

    if (local_crc32_plt_value != mem_crc32_plt_value) {
        isHook = true;
//        LOGE("plt hook detected");
    }
    close(fd);
    return isHook;
}

bool prettyMethodHookStatus() {
    bool isHook = false;
    SandHook::ElfImg libart("libart.so");
    // 本地文件查找的
    // _ZN3art9ArtMethod12PrettyMethodEb
    // _ZN3art9ArtMethod12PrettyMethodEPS0_b

    // https://github1s.com/LSPosed/LSPlant/blob/master/lsplant/src/main/jni/art/runtime/art_method.cxx#L27
    // lsposed 源码中的，但是在测试中只有第一个可以找到相应的函数，所以还是需要与本地文件相结合来看
    // _ZN3art9ArtMethod12PrettyMethodEPS0_b
    // _ZN3art12PrettyMethodEPNS_9ArtMethodEb
    // _ZN3art12PrettyMethodEPNS_6mirror9ArtMethodEb
    std::array<const char *, 4> symbols = {
            "_ZN3art9ArtMethod12PrettyMethodEb",
            "_ZN3art9ArtMethod12PrettyMethodEPS0_b",
            "_ZN3art12PrettyMethodEPNS_9ArtMethodEb",
            "_ZN3art12PrettyMethodEPNS_6mirror9ArtMethodEb"
    };

    void *PrettyMethod_addr = nullptr;
    for (const auto &symbol: symbols) {
        PrettyMethod_addr = libart.getSymbAddress(symbol);
        if (PrettyMethod_addr != nullptr) {
            break;
        }
    }

    if (PrettyMethod_addr == nullptr) {
        LOGI("PrettyMethod not found");
    }

    uintptr_t PrettyMethod_offset = (uintptr_t) PrettyMethod_addr - (uintptr_t) libart.getBase();
    LOGI("PrettyMethod addr: 0x%x", PrettyMethod_offset);

    // 根据 PrettyMethod_offset 读取本地 libart PrettyMethod 的前16字节进行 CRC32 计算
    int fd = open(libart.name().c_str(), O_RDONLY);
    lseek(fd, PrettyMethod_offset, 0);
    char buf[16];
    read(fd, buf, 16);
    uintptr_t local_crc32_prettymethod_value = calculateCRC32(
            reinterpret_cast<const unsigned char *>(buf), 16);

    // 读取内存中的 PrettyMethod 前16字节进行 CRC32 计算
    uintptr_t mem_crc32_prettymethod_value = calculateCRC32(
            reinterpret_cast<const unsigned char *>(PrettyMethod_addr), 16);

    LOGI("local PrettyMethod crc32: 0x%x, mem PrettyMethod crc32: 0x%x",
         local_crc32_prettymethod_value, mem_crc32_prettymethod_value);
    if (local_crc32_prettymethod_value != mem_crc32_prettymethod_value) {
        isHook = true;
//        LOGE("PrettyMethod hook detected");
    }
    close(fd);
    return isHook;
}

int callStackDetection(JNIEnv *env) {
    // 使用 "../include/obfs-string.h" 的字符串混淆功能
    SandHook::ElfImg libart("libart.so");

    // 获取 libart nativeFillInStackTrace，进而获取 javaStackState
    void *nativeFillInStackTrace = libart.getSymbAddress(
            "_ZN3artL32Throwable_nativeFillInStackTraceEP7_JNIEnvP7_jclass");

    jobject(*my_nativeFillInStackTrace)(JNIEnv * , jclass); // 函数指针
    my_nativeFillInStackTrace = reinterpret_cast<jobject (*)(JNIEnv *,
                                                             jclass)>(nativeFillInStackTrace);
    jclass MainActivity = env->FindClass("com/nuthecz/monitordevice/CheckHookActivity");
    jobject javaStackState = my_nativeFillInStackTrace(env, MainActivity);

    // 获取 libart nativeGetStackTrace，获取 objAry
    void *nativeGetStackTrace = libart.getSymbAddress(
            "_ZN3artL29Throwable_nativeGetStackTraceEP7_JNIEnvP7_jclassP8_jobject");

    jobjectArray(*my_nativeGetStackTrace)(JNIEnv * , jclass, jobject); // 函数指针
    my_nativeGetStackTrace = reinterpret_cast<jobjectArray (*)(JNIEnv *, jclass,
                                                               jobject)>(nativeGetStackTrace);
    jobjectArray objAry = my_nativeGetStackTrace(env, MainActivity, javaStackState);

    jsize length = env->GetArrayLength(objAry);

    std::list <std::string> myList;
    std::set <std::string> mySet;

    // 遍历 objAry， 获取 className, methodName, fileName, lineNumber
    for (int i = 0; i < length; i++) {
        jobject obj = env->GetObjectArrayElement(objAry, i);
        jclass clazz = env->GetObjectClass(obj);
        jstring className = (jstring) env->CallObjectMethod(
                obj, env->GetMethodID(clazz, "getClassName", "()Ljava/lang/String;"));
        jstring methodName = (jstring) env->CallObjectMethod(
                obj, env->GetMethodID(clazz, "getMethodName", "()Ljava/lang/String;"));
        jstring fileName = (jstring) env->CallObjectMethod(
                obj, env->GetMethodID(clazz, "getFileName", "()Ljava/lang/String;"));

        int lineNumber = env->CallIntMethod(
                obj, env->GetMethodID(clazz, "getLineNumber", "()I"));

        // 如果 fileName 为空，则说明是 lsposed 的 hook
        if (className != nullptr && methodName != nullptr && fileName != nullptr) {
            LOGI("%s -> %s(%s: %d)", env->GetStringUTFChars(className, 0),
                 env->GetStringUTFChars(methodName, 0), env->GetStringUTFChars(fileName, 0),
                 lineNumber);
            char des[128];
            sprintf(des, "%s -> %s(%s: %d)", env->GetStringUTFChars(className, 0),
                    env->GetStringUTFChars(methodName, 0), env->GetStringUTFChars(fileName, 0),
                    lineNumber);
            myList.push_back(des);
            mySet.insert(des);
        } else {
            LOGE("xposed hook detected");
            return 2;
        }
    }

    // 如果存在重复的元素，说明可能是 frida hook
    if (myList.size() == mySet.size()) {
        std::string last = myList.back();
        // 如果函数调用不是从默认方法开始调用的，则说明是 rpc hook(比如使用 frida 远程调用函数)
        if (strstr(last.c_str(), "com.android.internal.os.ZygoteInit -> main") ||
            strstr(last.c_str(), "java.lang.Thread -> run")) {
            LOGI("all is right");
            return 0;
        } else {
            LOGE("rpc hook detected");
            return 1;
        }
    } else {
        LOGE("frida hook detected");
        return 3;
    }
}

bool frida_check_maps() {
    char line[512];
    FILE *fp = fopen("/proc/self/maps", "r");
    if (fp) {
        // 如果文件成功打开，循环读取每一行
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, "frida") || strstr(line, "gadget")) {
                fclose(fp);
                return true;
            }
        }
        fclose(fp);
    } else {
        // 如果无法打开文件，记录错误。这可能意味着系统状态异常
        LOGE("Do not open the file of maps");
    }
    return false;
}


extern "C" int
my_openat_svc(int dirfd, const char *const __pass_object_size pathname, int flags, mode_t modes);

bool frida_check_maps_self() {
//    int fd = my_openat("/proc/self/maps");
    int fd = my_openat_svc(AT_FDCWD, "/proc/self/maps", O_RDONLY | O_CLOEXEC, 0);
    if (fd == -1) {
        LOGE("Failed to open /proc/self/maps");
        return false;
    }

    char buffer[512]; // 假设每行不超过512字节
    char *line = NULL;
    ssize_t bytesRead;
    ssize_t bytesWritten = 0;

    while (true) {
        bytesRead = read(fd, buffer, sizeof(buffer) - 1);
        if (bytesRead == -1) {
            LOGE("Error reading from file");
            break;
        }
        if (bytesRead == 0) {
            break; // 文件结束
        }

        buffer[bytesRead] = '\0'; // 确保字符串以'\0'结尾
        line = buffer;
        // 查找关键字
        if (local_strstr(line, "frida") || local_strstr(line, "gadget")) {
            close(fd);
            return true;
        }

        // 寻找换行符的位置
        char *newline = local_strchr(line, '\n');
        if (newline != NULL) {
            *newline = '\0'; // 截断行
            bytesWritten = write(STDOUT_FILENO, line, strlen(line));
            *newline = '\n'; // 恢复换行符
            bytesWritten += write(STDOUT_FILENO, newline, 1);
        } else {
            // 如果没有找到换行符，则继续读取直到找到换行符或文件结束
            continue;
        }
    }
    close(fd);
    return false;
}

bool frida_check_status() {
    DIR *dir = opendir("/proc/self/task/");
    struct dirent *entry;
    char status_path[MAX_PATH];
    char buffer[MAX_BUFFER];
    int found = false;

    if (dir) {
        // 遍历目录中的每个条目
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR) {
                // 忽略.和..目录
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                    continue;
                }
                // 构造每个线程的状态文件路径，然后读取内容
                snprintf(status_path, sizeof(status_path), "/proc/self/task/%s/status",
                         entry->d_name);
                if (local_readfile(status_path, buffer, sizeof(buffer)) == -1) {
                    continue;
                }
                if (strcmp(buffer, "null") == 0) {
                    continue;
                }
                // 分割缓冲区内容按行处理
                char *line = strtok(buffer, "\n");
                while (line) {
                    if (strstr(line, "Name:") != NULL) {
                        // 检查线程名称是否包含 Frida 相关的字符串
                        const char *frida_name = strstr(line, "gmain");
                        if (frida_name || strstr(line, "gum-js-loop") ||
                            strstr(line, "pool-frida") || strstr(line, "gdbus")) {
                            found = true;
                            break;
                        }
                    }
                    line = strtok(NULL, "\n");
                }
                if (found) break;
            }
        }
        closedir(dir);
    }
    return found;
}

bool xposed_check_maps(char *path) {
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

bool xposed_check_stack(JNIEnv *env) {
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