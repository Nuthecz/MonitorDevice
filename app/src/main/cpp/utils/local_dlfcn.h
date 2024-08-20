#ifndef LOCAL_DLFCN_H
#define LOCAL_DLFCN_H

#include <fcntl.h>
#include <sys/mman.h>
#include <elf.h>
#include <sys/syscall.h>

#ifdef __cplusplus
extern "C" {
#endif

struct local_dlfcn_handle {
    char *elf;
    size_t size;
    char *dynsym;
    int nsyms;
    char *dynstr;
};

void *local_dlopen(const char *lib_path);
off_t local_dlsym(void *handle, const char *sym_name);
void local_dlclose(void *handle);
// 这里可以加上 static inline 进行内联操作，不过这样做会明确其内部链接属性，那么就必须放在同一个文件中，否则编译器会报错
char *local_strchr(const char *s, const char ch);
bool local_strstr(const char *str1, const char *str2);
int my_openat(const char *pathname);
ssize_t local_readfile(const char *path, char *buffer, size_t size);

#ifdef __cplusplus
}
#endif

#endif
