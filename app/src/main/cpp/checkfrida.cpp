#include <jni.h>
#include <string.h>
#include <dirent.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "basic.h"
#include "dlfcn/local_dlfcn.h"
#define MAX_PATH 256
#define MAX_BUFFER 1024

bool check_maps() {
    char line[512];
    FILE* fp = fopen("/proc/self/maps", "r");
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
my_openat(int dirfd, const char *const __pass_object_size pathname, int flags, mode_t modes);

__attribute__((always_inline))
static inline int my_openat1(const char *pathname) {
    int fd;
    fd = syscall(SYS_openat, AT_FDCWD, pathname, O_RDONLY, 0);
    if (fd == -1) {
        return -1;
    }
    return fd;
}
//    int fd = my_openat(AT_FDCWD, "/proc/self/maps", O_RDONLY | O_CLOEXEC, 0);


__attribute__((always_inline))
static inline char *my_strchr(const char *s, const char ch)
{
    if (NULL == s)
        return NULL;

    const char *pSrc = s;
    while ('\0' != *pSrc)
    {
        if (*pSrc == ch)
        {
            return (char *)pSrc;
        }
        ++ pSrc;
    }
    return NULL;
}

__attribute__((always_inline))
static inline bool my_strstr(const char *str1, const char *str2)
{
    char *cp = (char *)str1;
    char *s1, *s2;
    if (!*str2)
        return (char *)str1;
    while (*cp)
    {
        s1 = cp;
        s2 = (char *)str2;
        while (*s2 && !(*s1 - *s2))
            s1++, s2++;
        if (!*s2)
            return cp;
        cp++;
    }
    return false;
}

bool check_maps_self() {
    int fd = my_openat1("/proc/self/maps");
    if (fd == -1) {
        LOGE("Failed to open /proc/self/maps");
        return false;
    }

    char buffer[512]; // 假设每行不超过512字节
    char *line = NULL;
    size_t len = 0;
    ssize_t bytesRead;
    ssize_t bytesWritten = 0;
    bool found = false;

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
        if (my_strstr(line, "frida") || my_strstr(line, "gadget")) {
            close(fd);
            return true;
        }·


        // 寻找换行符的位置
        char *newline = my_strchr(line, '\n');
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

ssize_t read_file(const char *path, char *buffer, size_t size) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) return -1;
    ssize_t bytesRead = read(fd, buffer, size - 1);
    buffer[bytesRead] = '\0';
    close(fd);
    return bytesRead;
}

bool check_status() {
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
                snprintf(status_path, sizeof(status_path), "/proc/self/task/%s/status", entry->d_name);
                if (read_file(status_path, buffer, sizeof(buffer)) == -1) {
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
                        if (frida_name || strstr(line, "gum-js-loop") || strstr(line, "pool-frida") || strstr(line, "gdbus")) {
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

bool check_inlinehook() {
    // 根据系统架构选择对应的libc.so库路径
    const char *lib_path;
#ifdef __LP64__
    lib_path = "/system/lib64/libc.so";
#else
    lib_path = "/system/lib/libc.so";
#endif

    // 定义要比较的字节数
    const int CMP_COUNT = 8;
    // 指定要查找的符号名，这里是"open"函数
    const char *sym_name = "open";

    // 使用local_dlopen函数打开指定的共享库，并获取操作句柄
    struct local_dlfcn_handle *handle = static_cast<local_dlfcn_handle *>(local_dlopen(lib_path));
    if (!handle) {
        return JNI_FALSE; // 如果无法打开共享库，返回false
    }

    // 获取"open"函数在libc.so中的偏移量
    off_t offset = local_dlsym(handle, sym_name);

    // 关闭handle，因为我们接下来使用标准的dlopen/dlsy来获取函数地址
    local_dlclose(handle);

    // 打开libc.so文件，准备读取数据
    FILE *fp = fopen(lib_path, "rb");
    if (!fp) {
        return JNI_FALSE; // 如果无法打开文件，返回false
    }

    // 定义一个缓冲区，用于存储读取的文件内容
    char file_bytes[CMP_COUNT] = {0};
    // 读取指定偏移量处的CMP_COUNT个字节
    fseek(fp, offset, SEEK_SET);
    fread(file_bytes, 1, CMP_COUNT, fp);
    fclose(fp);

    // 使用dlopen函数打开libc.so共享库，并获取操作句柄
    void *dl_handle = dlopen(lib_path, RTLD_NOW);
    if (!dl_handle) {
        return JNI_FALSE; // 如果无法打开共享库，返回false
    }

    // 使用dlsym函数获取"open"函数的地址
    void *sym = dlsym(dl_handle, sym_name);
    if (!sym) {
        dlclose(dl_handle);
        return JNI_FALSE; // 如果无法找到符号，返回false
    }

    // 比较原libc.so中的"open"函数内容与通过dlsym获取的"open"函数内容是否一致
    int is_hook = memcmp(file_bytes, sym, CMP_COUNT) != 0;

    // 关闭dlopen打开的共享库句柄
    dlclose(dl_handle);

    // 返回比较结果，如果函数被hook则返回JNI_TRUE，否则返回JNI_FALSE
    return is_hook ? JNI_TRUE : JNI_FALSE;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_checkfrida_FridaCheck_checkFrida(
        JNIEnv *env,
        jobject /* this */) {
    if(check_status()){
        LOGI("Detected Frida!!!(So)");
        LOGI("Status detected");
    }
    if(check_maps()){
        LOGI("Detected Frida!!!(So)");
        LOGI("Maps detected");
    }
    if(check_maps_self()){
        LOGI("Detected Frida!!!(So)");
        LOGI("Maps Self-fulfillment detected");
    }
    if(check_inlinehook()){
        LOGI("Detected Frida!!!(So)");
        LOGI("InlineHook detected");
    }
}