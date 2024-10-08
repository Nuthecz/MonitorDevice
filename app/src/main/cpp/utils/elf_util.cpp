//
// Created by NU on 2024/10/8.
//

#include <malloc.h>
#include <cstring>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cassert>
#include <sys/stat.h>
#include "../include/elf_util.h"
#include "../include/config.h"

using namespace SandHook;

template<typename T>
inline constexpr auto offsetOf(ElfW(Ehdr) * head, ElfW(Off)
off) {
return reinterpret_cast<std::conditional_t<std::is_pointer_v < T>, T, T *>>(
reinterpret_cast
<uintptr_t>(head)
+ off);
}

ElfImg::ElfImg(std::string_view base_name) : elf(base_name) {
    if (!findModuleBase()) {
        base = nullptr;
        return;
    }

    //load elf
    int fd = open(elf.data(), O_RDONLY);
    if (fd < 0) {
        LOGE("failed to open {%s}", elf.c_str());
        return;
    }

    size = lseek(fd, 0, SEEK_END);
    if (size <= 0) {
        LOGE("lseek() failed for {%s}", elf.c_str());
    }

    header = reinterpret_cast<decltype(header) >
                              (mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0));

    close(fd);

    section_header = offsetOf<decltype(section_header)>(header, header->e_shoff);

    auto shoff = reinterpret_cast<uintptr_t>(section_header);
    char *section_str = offsetOf<char *>(header, section_header[header->e_shstrndx].sh_offset);

    for (int i = 0; i < header->e_shnum; i++, shoff += header->e_shentsize) {
        auto *section_h = (ElfW(Shdr) * )
        shoff;
        char *sname = section_h->sh_name + section_str;
        auto entsize = section_h->sh_entsize;
        switch (section_h->sh_type) {
            case SHT_DYNSYM: {
                if (bias == -4396) {
                    dynsym = section_h;
                    dynsym_offset = section_h->sh_offset;
                    dynsym_start = offsetOf<decltype(dynsym_start)>(header, dynsym_offset);
                }
                break;
            }
            case SHT_SYMTAB: {
                if (strcmp(sname, ".symtab") == 0) {
                    symtab = section_h;
                    symtab_offset = section_h->sh_offset;
                    symtab_size = section_h->sh_size;
                    symtab_count = symtab_size / entsize;
                    symtab_start = offsetOf<decltype(symtab_start)>(header, symtab_offset);
                }
                break;
            }
            case SHT_STRTAB: {
                if (bias == -4396) {
                    strtab = section_h;
                    symstr_offset = section_h->sh_offset;
                    strtab_start = offsetOf<decltype(strtab_start)>(header, symstr_offset);
                }
                if (strcmp(sname, ".strtab") == 0) {
                    symstr_offset_for_symtab = section_h->sh_offset;
                }
                break;
            }
            case SHT_PROGBITS: {
                if (strtab == nullptr || dynsym == nullptr) break;
                if (bias == -4396) {
                    bias = (off_t) section_h->sh_addr - (off_t) section_h->sh_offset;
                }
                break;
            }
            case SHT_HASH: {
                auto *d_un = offsetOf<ElfW(Word)>(header, section_h->sh_offset);
                nbucket_ = d_un[0];
                bucket_ = d_un + 2;
                chain_ = bucket_ + nbucket_;
                break;
            }
            case SHT_GNU_HASH: {
                auto *d_buf = reinterpret_cast<ElfW(Word) * > (((size_t) header) +
                                                               section_h->sh_offset);
                gnu_nbucket_ = d_buf[0];
                gnu_symndx_ = d_buf[1];
                gnu_bloom_size_ = d_buf[2];
                gnu_shift2_ = d_buf[3];
                gnu_bloom_filter_ = reinterpret_cast<decltype(gnu_bloom_filter_) > (d_buf + 4);
                gnu_bucket_ = reinterpret_cast<decltype(gnu_bucket_) > (gnu_bloom_filter_ +
                                                                        gnu_bloom_size_);
                gnu_chain_ = gnu_bucket_ + gnu_nbucket_ - gnu_symndx_;
                break;
            }
        }
        // 新添加获取段信息
        if (strcmp(sname, ".text") == 0) {
            text_section_offset = section_h->sh_offset;
            text_section_size = section_h->sh_size;
        }
        if (strcmp(sname, ".plt") == 0) {
            plt_section_offset = section_h->sh_offset;
            plt_section_size = section_h->sh_size;
        }

    }
}

ElfW(Addr)
ElfImg::ElfLookup(std::string_view
name,
uint32_t hash
) const {
if (nbucket_ == 0) return 0;

char *strings = (char *) strtab_start;

for (
auto n = bucket_[hash % nbucket_];
n != 0;
n = chain_[n]
) {
auto *sym = dynsym_start + n;
if (name == strings + sym->st_name) {
return sym->
st_value;
}
}
return 0;
}

ElfW(Addr)
ElfImg::GnuLookup(std::string_view
name,
uint32_t hash
) const {
static constexpr auto bloom_mask_bits = sizeof(ElfW(Addr)) * 8;

if (gnu_nbucket_ == 0 || gnu_bloom_size_ == 0) return 0;

auto bloom_word = gnu_bloom_filter_[(hash / bloom_mask_bits) % gnu_bloom_size_];
uintptr_t mask = 0
                 | (uintptr_t) 1 << (hash % bloom_mask_bits)
                 | (uintptr_t) 1 << ((hash >> gnu_shift2_) % bloom_mask_bits);

if ((
mask &bloom_word
) == mask) {
auto sym_index = gnu_bucket_[hash % gnu_nbucket_];
if (sym_index >= gnu_symndx_) {
char *strings = (char *) strtab_start;
do {
auto *sym = dynsym_start + sym_index;
if (((gnu_chain_[sym_index] ^ hash) >> 1) == 0
&& name == strings + sym->st_name) {
return sym->
st_value;
}
} while ((gnu_chain_[sym_index++] & 1) == 0);
}
}

return 0;
}

void ElfImg::MayInitLinearMap() const {
    if (symtabs_.empty()) {
        if (symtab_start != nullptr && symstr_offset_for_symtab != 0) {
            for (ElfW(Off) i = 0; i < symtab_count;
            i++) {
                unsigned int st_type = ELF_ST_TYPE(symtab_start[i].st_info);
                const char *st_name = offsetOf<const char *>(header, symstr_offset_for_symtab +
                                                                     symtab_start[i].st_name);
                if ((st_type == STT_FUNC || st_type == STT_OBJECT) && symtab_start[i].st_size) {
                    symtabs_.emplace(st_name, &symtab_start[i]);
                }
            }
        }
    }
}

ElfW(Addr)
ElfImg::LinearLookup(std::string_view
name) const {
MayInitLinearMap();

if (
auto i = symtabs_.find(name);
i != symtabs_.

end()

) {
return i->second->
st_value;
} else {
return 0;
}
}

std::vector<ElfW(Addr)> ElfImg::LinearRangeLookup(std::string_view name) const {
    MayInitLinearMap();
    std::vector < ElfW(Addr) > res;
    for (auto [i, end] = symtabs_.equal_range(name); i != end; ++i) {
        auto offset = i->second->st_value;
        res.emplace_back(offset);
        LOGD("found {%s} {0x%x} in {%s} in symtab by linear range lookup",
             std::string(name).c_str(), offset, elf.c_str());
    }
    return res;
}

ElfW(Addr)
ElfImg::PrefixLookupFirst(std::string_view
prefix) const {
MayInitLinearMap();

if (
auto i = symtabs_.lower_bound(prefix);
i != symtabs_.

end() &&

i->first.
starts_with(prefix)
) {
LOGD("found prefix {%s} of {%s} {0x%x} in {%s} in symtab by linear lookup",
     std::string(prefix).c_str(), std::string(i->first).c_str(), i->second->st_value, elf.c_str());
return i->second->
st_value;
} else {
return 0;
}
}


ElfImg::~ElfImg() {
    //open elf file local
    if (buffer) {
        free(buffer);
        buffer = nullptr;
    }
    //use mmap
    if (header) {
        munmap(header, size);
    }
}

ElfW(Addr)
ElfImg::getSymbOffset(std::string_view
name,
uint32_t gnu_hash, uint32_t
elf_hash) const {
if (
auto offset = GnuLookup(name, gnu_hash);
offset > 0) {
LOGD("found {%s} {0x%x} in {%s} in dynsym by gnuhash", std::string(name).c_str(), offset,
     elf.c_str());
return
offset;
} else if (
offset = ElfLookup(name, elf_hash);
offset > 0) {
LOGD("found {%s} {0x%x} in {%s} in dynsym by gnuhash", std::string(name).c_str(), offset,
     elf.c_str());
return
offset;
} else if (
offset = LinearLookup(name);
offset > 0) {
LOGD("found {%s} {0x%x} in {%s} in symtab by linear lookup", std::string(name).c_str(), offset,
     elf.c_str());
return
offset;
} else {
return 0;
}
// 报错说明：这里贸然按照 {%s} name 打印会报错，Fatal signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr 0x2acd4 in tid 6490 (.nuthecz.gather), pid 6490 (.nuthecz.gather)
// 这是因为打印的格式不对，所以引发了错误，要回看 backtrace，下面栈回溯指出了错误在 (__android_log_print+144) 中，那么就是 log 有问题
// 所以需要先转化类型再按照类型占位符进行打印
//    backtrace:
//      #00 pc 000000000001e2a0  /system/lib64/libc.so (strlen+16)
//      #01 pc 000000000005f034  /system/lib64/libc.so (__vfprintf+6092)
//      #02 pc 000000000007d8b4  /system/lib64/libc.so (vsnprintf+164)
//      #03 pc 0000000000046f24  /system/lib64/libc.so (__vsnprintf_chk+72)
//      #04 pc 0000000000007e50  /system/lib64/liblog.so (__android_log_print+144)
//      #05 pc 000000000002d62c  /data/app/com.nuthecz.gather-4M_li-lWMKucUgAlxwlxfA==/lib/arm64/libgather.so (SandHook::ElfImg::getSymbOffset(std::__ndk1::basic_string_view<char, std::__ndk1::char_traits<char>>, unsigned int, unsigned int) const+180)
}

constexpr inline bool contains(std::string_view a, std::string_view b) {
    return a.find(b) != std::string_view::npos;
}

bool ElfImg::findModuleBase() {
    off_t load_addr;
    bool found = false;
    FILE *maps = fopen("/proc/self/maps", "r");

    char *buff = nullptr;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&buff, &len, maps)) != -1) {
        std::string_view line{buff, static_cast<size_t>(nread)};

        if ((contains(line, "r-xp") || contains(line, "r--p")) && contains(line, elf)) {
            LOGD("found: {%s}", std::string(line).c_str());
            if (auto begin = line.find_last_of(' '); begin != std::string_view::npos &&
                                                     line[++begin] == '/') {
                found = true;
                elf = line.substr(begin);
                if (elf.back() == '\n') elf.pop_back();
                LOGD("update path: {%s}", elf.c_str());
                break;
            }
        }
    }
    if (!found) {
        if (buff) free(buff);
        LOGE("failed to read load address for: {%s}", elf.c_str());
        fclose(maps);
        return false;
    }

    if (char *next = buff; load_addr = strtoul(buff, &next, 16), next == buff) {
        LOGE("failed to read load address for: {%s}", elf.c_str());
    }

    if (buff) free(buff);

    fclose(maps);

    LOGD("get module base: {%s} -> {0x%lx}", elf.c_str(), load_addr);

    base = reinterpret_cast<void *>(load_addr);
    return true;
}