#include "utils/basic.h"
#include <linux/seccomp.h>
#include <linux/filter.h>
#include <sys/prctl.h>
#include <linux/unistd.h>
#include <dirent.h>
#include <dlfcn.h>

#define AUDIT_ARCH_I386 0x40000003
#define AUDIT_ARCH_X86_64 0xC000003E

// 通过 seccomp 检测架构，如果为 x86 或者 x86_64，就判断为模拟器
// 通过调用 __NR_getpid 进行检测，当调用这个系统调用时，进行 ABI 的检测，如果是 arm 架构的，返回报错 6，否则返回对应设置的报错码
void install_check_arch_seccomp() {
    struct sock_filter filter[12] = {
            BPF_STMT(BPF_LD | BPF_W | BPF_ABS,
                     (uint32_t) offsetof(struct seccomp_data, nr)), // 设置 data 到寄存器
            BPF_JUMP(BPF_JMP | BPF_JEQ, __NR_getpid, 0, 3), // 相等执行下一条指令，不等跳过下面10条指令

            BPF_STMT(BPF_LD | BPF_W | BPF_ABS, (uint32_t) offsetof(struct seccomp_data, arch)),
            BPF_JUMP(BPF_JMP | BPF_JEQ, AUDIT_ARCH_I386, 0, 2),
            BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | (300 & SECCOMP_RET_DATA)),
            BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),

            BPF_STMT(BPF_LD | BPF_W | BPF_ABS, (uint32_t) offsetof(struct seccomp_data, arch)),
            BPF_JUMP(BPF_JMP | BPF_JEQ, AUDIT_ARCH_X86_64, 0, 2),
            BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | (600 & SECCOMP_RET_DATA)),
            BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),

            BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | (6 & SECCOMP_RET_DATA)),
            BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW)
    };
    //    上面使用 seccomp-tools 显示的约束条件
    //    line  CODE  JT   JF      K
    //    =================================
    //    0000: 0x20 0x00 0x00 0x00000000  A = sys_number
    //    0001: 0x15 0x00 0x03 0x00000027  if (A != getpid) goto 0005
    //    0002: 0x20 0x00 0x00 0x00000004  A = arch
    //    0003: 0x15 0x00 0x02 0x40000003  if (A != ARCH_I386) goto 0006
    //    0004: 0x06 0x00 0x00 0x0005012c  return ERRNO(300)
    //    0005: 0x06 0x00 0x00 0x7fff0000  return ALLOW
    //    0006: 0x20 0x00 0x00 0x00000004  A = arch
    //    0007: 0x15 0x00 0x02 0xc000003e  if (A != ARCH_X86_64) goto 0010
    //    0008: 0x06 0x00 0x00 0x00050258  return ERRNO(600)
    //    0009: 0x06 0x00 0x00 0x7fff0000  return ALLOW
    //    0010: 0x06 0x00 0x00 0x00050006  return ERRNO(6)
    //    0011: 0x06 0x00 0x00 0x7fff0000  return ALLOW

    struct sock_fprog program = {
            .len = (unsigned short) (sizeof(filter) / sizeof(filter[0])),
            .filter = filter
    };
    // 配置初始环境
    errno = 0;
    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) {
        LOGE("prctl(PR_SET_NO_NEW_PRIVS): %s", strerror(errno));
    }
    // 启用上述规则
    errno = 0;
    if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &program)) {
        LOGE("prctl(PR_SET_SECCOMP): %s", strerror(errno));
    }
}

std::string check_arch_by_seccomp(JNIEnv *env) {
    install_check_arch_seccomp();
    errno = 0;
    syscall(__NR_getpid);
    if (errno == 300) {
        LOGI("Detected Emu!!!(So)");
        LOGI("x86 Architecture Detected");
        return "Simulator ARCH Detected";
    } else if (errno == 600) {
        LOGI("Detected Emu!!!(So)");
        LOGI("X86_64 Architecture Detected");
        return "Simulator ARCH Detected";
    }
    return "";
}

bool file_exist(const char *path) {
    return (access(path, F_OK) == 0);
}

// 检测模拟器特征文件
bool simulator_files_check() {
    if (file_exist("/system/bin/androVM-prop")) {  //检测androidVM
        return true;
    } else if (file_exist("/system/bin/microvirt-prop")) { //检测逍遥模拟器--新版本找不到特征
        return true;
    } else if (file_exist("/system/lib/libdroid4x.so")) { //检测海马模拟器
        return true;
    } else if (file_exist("/system/bin/windroyed")) { //检测文卓爷模拟器
        return true;
    } else if (file_exist("/system/bin/nox-prop")) { //检测夜神模拟器--某些版本找不到特征
        return true;
    } else if (file_exist("system/lib/libnoxspeedup.so")) { //检测夜神模拟器
        return true;
    } else if (file_exist("/system/bin/ttVM-prop")) { //检测天天模拟器
        return true;
    } else if (file_exist("/data/.bluestacks.prop")) { //检测bluestacks模拟器  51模拟器
        return true;
    } else if (file_exist("/system/bin/duosconfig")) { //检测AMIDuOS模拟器
        return true;
    } else if (file_exist("/system/etc/xxzs_prop.sh")) { //检测星星模拟器
        return true;
    } else if (file_exist("/system/etc/mumu-configs/device-prop-configs/mumu.config")) { //网易MuMu模拟器
        return true;
    } else if (file_exist("/system/priv-app/ldAppStore")) { //雷电模拟器
        return true;
    } else if (file_exist("system/bin/ldinit") && file_exist("system/bin/ldmountsf")) { //雷电模拟器
        return true;
    } else if (file_exist("/system/app/AntStore") &&
               file_exist("/system/app/AntLauncher")) { //小蚁模拟器
        return true;
    } else if (file_exist("vmos.prop")) { //vmos虚拟机
        return true;
    } else if (file_exist("fstab.titan") && file_exist("init.titan.rc")) { //光速虚拟机
        return true;
    } else if (file_exist("x8.prop")) { // x8沙箱和51虚拟机
        return true;
    } else if (file_exist("/system/lib/libc_malloc_debug_qemu.so")) { // AVD QEMU
        return true;
    }
    return false;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_checkenv_EmulatorCheck_checkEmu(JNIEnv *env, jobject thiz) {
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
JNIEXPORT jstring JNICALL
Java_com_example_checkenv_SandboxCheck_checkSandbox(JNIEnv *env, jobject thiz) {
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
        LOGI("Detected Sandbox!!!(File)");
        LOGI("Sandbox Detected");
        result += "Sandbox Detected";
    }
    return env->NewStringUTF(result.c_str());
}