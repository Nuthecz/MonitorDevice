#include "utils/basic.h"
#include "utils/local_dlfcn.h"
#include <dlfcn.h>

using namespace std;

int getSdk() {
    char sdk_version[32] = {0};
    __system_property_get("ro.build.version.sdk", sdk_version);
    int SDK_INT = -1;
    SDK_INT = stoi(sdk_version);
    return SDK_INT;
}

#if defined(__LP64__)
typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Shdr Elf_Shdr;
typedef Elf64_Addr Elf_Addr;
typedef Elf64_Dyn Elf_Dyn;
typedef Elf64_Rela Elf_Rela;
typedef Elf64_Sym Elf_Sym;
typedef Elf64_Off Elf_Off;

#define ELF_R_SYM(i) ELF64_R_SYM(i)

#else
typedef Elf32_Ehdr Elf_Ehdr;
typedef Elf32_Shdr Elf_Shdr;
typedef Elf32_Addr Elf_Addr;
typedef Elf32_Dyn Elf_Dyn;
typedef Elf32_Rel Elf_Rela;
typedef Elf32_Sym Elf_Sym;
typedef Elf32_Off Elf_Off;

#define ELF_R_SYM(i) ELF32_R_SYM(i)
#endif

char *getlibc() {
    int SDK_INT = -1;
    SDK_INT = getSdk();
    if (SDK_INT == -1) {
        LOGI("sdk search Erro!");
    }
#if defined(__aarch64__) //lib与lib64的区别
    char *libc;
    if (SDK_INT >= 29) {
        libc = (char *) "/apex/com.android.runtime/lib64/bionic/libc.so";
    } else {
        libc = (char *) "/system/lib64/libc.so";
    }
#else
    char *libc;
    if (SDK_INT >= 30) {//Android 10（API 级别 29）及以上版本中，Google 引入了 APEX（Android Project Execution Environment）作为操作系统模块化的一部分，用以更新和改进系统核心组件,系统运行时和库已经逐渐迁移到 APEX 模块中.
        libc = (char *) "/apex/com.android.runtime/lib/bionic/libc.so";
    } else if (SDK_INT >= 29) {
        libc = (char *) "/apex/com.android.runtime/lib/bionic/libc.so";
    } else {
        libc = (char *) "/system/lib/libc.so";
    }
#endif
    return libc;
}

// 计算 buffer 中所有字节的累加和
static unsigned long checkSum(void *buffer, size_t len) {
    if (buffer == nullptr || len < 100) {
        return 0;
    }
    unsigned long seed = 0;
    auto *buf = (uint8_t *) buffer;
    for (size_t i = 0; i < len; i++) {
        uint8_t *ptr = buf++;
        seed += (unsigned long) (*ptr);
    };
    return seed;
}

// 获取节区表校验和
sectionResult getSectionCheckSum(const char *path) {
    sectionResult section;

    // 打开 libc.so 文件
    Elf_Ehdr elfEhdr;
    FILE *fp;
    fp = fopen(path, "r");
    if (NULL == fp) {
        LOGI("Failed to open file");
        return section;
    }

    // 读取文件头
    if(fread(&elfEhdr, sizeof(Elf_Ehdr), 1, fp) == 0) {
        LOGI("Failed to read file");
        return section;
    }
    // 解析 Section Header
    Elf_Shdr *elfShdr = (Elf_Shdr*)malloc(sizeof(Elf64_Shdr) * elfEhdr.e_shnum);
    if(fseek(fp, elfEhdr.e_shoff, SEEK_SET) != 0) {
        LOGI("Failed to seek file");
        return section;
    }
    // 读取所有Segment Header 到 phdr, 大小为sizeof(Elf64_Phdr) * 数量
    if(fread(elfShdr, sizeof(Elf64_Shdr) * elfEhdr.e_shnum, 1, fp) == 0) {
        LOGI("Failed to read section");
        return section;
    }

    // 重置指针位置到文件流开头
    rewind(fp);
    // 将fp指针移到字符串表（.shstrtab）内容的偏移位置处
    fseek(fp, elfShdr[elfEhdr.e_shstrndx].sh_offset, SEEK_SET);
    // 把这个段的内容全部存储到 shstrtab 数组里面，e_shstrndx 项是字符串表（.shstrtab）的下标
    char shstrtab[elfShdr[elfEhdr.e_shstrndx].sh_size];

    // 读取内容
    if (0 == fread(shstrtab, elfShdr[elfEhdr.e_shstrndx].sh_size, 1, fp))
    {
        LOGI("faile to read");
    }

    // 遍历每一个节
    int j = 0;
    section.sectionNum = 0;
    for (int i = 0; i < elfEhdr.e_shnum; i++)
    {
        // sh_flags指示该section在进程执行时的特性，SHF_EXECINSTR(当前节包含可执行的机器指令)
        if (elfShdr[i].sh_flags & SHF_EXECINSTR){
            // shdr[i].sh_name 表示这个节名字在（.shstrtab）节内容中的偏移
            char * temp = shstrtab + elfShdr[i].sh_name;
            section.sectionName[j] = strdup(temp);
            section.offset[j] = elfShdr[i].sh_offset;
            section.memSize[j] = elfShdr[i].sh_size;
            section.sectionNum ++;
            j++;
            if (section.sectionNum == 2)break;
        }
    }

    // 这里 sectionNum = 2，一个 .plt 一个 .text
    for(int i = 0; i< section.sectionNum; i ++){
        fseek(fp, section.offset[i], SEEK_SET);
        auto buf = calloc(1, section.memSize[i] * sizeof(uint8_t));
        if (buf == nullptr) {
            free(buf);
            return section;
        }
        fread(buf, section.memSize[i],1,fp);
        section.checkSum[i] = checkSum(buf, section.memSize[i]);
        free(buf);
    }
    section.isCorrect = true;
    fclose(fp);
    return section;
}

int compareCheckSum(char *line, sectionResult *section, const char *pathLib) {
    unsigned long start;
    unsigned long end;
    char buf[32] = "";
    char path[256] = "";
    char tmp[128] = "";
    sscanf(line, "%lx-%lx %s %s %s %s", &start, &end, buf, tmp, tmp, tmp, path);
    if(buf[2] == 'x'){
        uint8_t *buffer = (uint8_t *) start;
        for (int i = 0; i < section->sectionNum; i++) {
            auto begin = (void *) (buffer + section->offset[i]);
            unsigned long size = section->memSize[i];
            unsigned long mapsSize = checkSum(begin, size);
            if (mapsSize != section->checkSum[i]) {
                LOGI("Detected Hook!!!(So)");
                LOGI("%s->%s InlineHook detected, local 0x%lx <--> map 0x%lx", pathLib,
                     section->sectionName[i], section->checkSum[i], mapsSize);
                return true;
            }
        }
    }
    return false;
}

int getCompareResult(sectionResult *section, const char *soName) {
    if (section == nullptr)
        return false;
    auto mapsPath = string("proc/").append(to_string(getpid())).append("/maps").c_str();
    FILE *maps = fopen(mapsPath, "r");
    if (!maps) {
        return false;
    }
    char line[256];
    int ret;
    // 遍历maps文件，获取libc.so的起始地址和结束地址
    while (fgets(line, sizeof(line), maps)) {
        if (strstr(line, soName) != nullptr) {
            ret = compareCheckSum(line, section, soName);
            if(ret) break;
        }
    }
    fclose(maps);
    return ret;
}

bool checkInlinehook(){
    char* libc_path = getlibc();
    // 获取本地的校验和
    auto localChecksum = getSectionCheckSum(libc_path);
    // 获取maps文件校验和并与本地进行比对
    int result = getCompareResult(&localChecksum, "libc.so");
    if (result) {
        return true;
    }
    return false;
}
bool checkOpenFunc(){
    // 根据系统架构选择对应的libc.so库路径
    const char* libc_path = getlibc();

    // 定义要比较的字节数
    const int CMP_COUNT = 8;
    // 指定要查找的符号名，这里是"open"函数
    const char *sym_name = "open";

    // 使用local_dlopen函数打开指定的共享库，并获取操作句柄
    struct local_dlfcn_handle *handle = static_cast<local_dlfcn_handle *>(local_dlopen(libc_path));
    if (!handle) {
        return JNI_FALSE; // 如果无法打开共享库，返回false
    }

    // 获取"open"函数在libc.so中的偏移量
    off_t offset = local_dlsym(handle, sym_name);

    // 关闭handle，因为我们接下来使用标准的dlopen/dlsy来获取函数地址
    local_dlclose(handle);

    // 打开libc.so文件，准备读取数据
    FILE *fp = fopen(libc_path, "rb");
    if (!fp) {
        return JNI_FALSE; // 如果无法打开文件，返回false
    }

    // 定义一个缓冲区，用于存储读取的文件内容
    char file_bytes[CMP_COUNT] = {0};
    // 读取指定偏移量处的CMP_COUNT个字节
    fseek(fp, offset, SEEK_SET);
    fread(file_bytes, 1, CMP_COUNT, fp);
    fclose(fp);

    // 使用 dlopen 函数打开 libc.so 共享库，并获取操作句柄
    void *dl_handle = dlopen(libc_path, RTLD_NOW);
    if (!dl_handle) {
        return JNI_FALSE; // 如果无法打开共享库，返回false
    }

    // 使用 dlsym 函数获取 "open" 函数的地址
    void *sym = dlsym(dl_handle, sym_name);
    if (!sym) {
        dlclose(dl_handle);
        return JNI_FALSE; // 如果无法找到符号，返回false
    }

    // 比较原 libc.so 中的 "open" 函数内容与通过 dlsym 获取的"open"函数内容是否一致
    int is_hook = memcmp(file_bytes, sym, CMP_COUNT) != 0;

    // 关闭 dlopen 打开的共享库句柄
    dlclose(dl_handle);

    // 返回比较结果，如果函数被hook则返回JNI_TRUE，否则返回JNI_FALSE
    return is_hook ? JNI_TRUE : JNI_FALSE;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_checkhook_HookCheck_checkHook(JNIEnv *env, jobject thiz) {
    std::string result;
    // 检测inlineHook，它比 frida 处的 inlineHook 更为强力
    // 这里只要曾经被 hook，那么它就会一直显示存在 hook 行为，除非手机重启
    if(checkInlinehook()){
        result = "There is inlineHook behavior";
    }

    if(checkOpenFunc()){
        if(!result.empty()) result += "\n";
        LOGI("libc.so -> Open Modified");
        result += "libc.so -> Open Modified";
    }

    if (result.empty()) {
        result = "No Hook Detected";
    }
    return env->NewStringUTF(result.c_str());
}
