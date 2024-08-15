#include <dirent.h>
#include <dlfcn.h>
#include "utils/basic.h"
#include "utils/local_dlfcn.h"

#define MAX_PATH 256
#define MAX_BUFFER 1024

bool check_maps() {
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

bool check_maps_self() {
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

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_checkhook_FridaCheck_checkFrida(
        JNIEnv *env,
        jobject /* this */) {
    std::string result;
    if (check_status()) {
        LOGI("Detected Frida!!!(So)");
        LOGI("Status Detected");
        result += "Status Detected";
    }

    if (check_maps()) {
        if(!result.empty()) result += "\n";
        LOGI("Detected Frida!!!(So)");
        LOGI("Maps Detected");
        result += "Maps Detected";
    }

    if (check_maps_self()) {
        if(!result.empty()) result += "\n";
        LOGI("Detected Frida!!!(So)");
        LOGI("Maps Self-fulfillment Detected");
        result += "Maps Self-fulfillment Detected";
    }

    if (result.empty()) {
        result = "No Frida Detected";
    }
    return env->NewStringUTF(result.c_str());
}