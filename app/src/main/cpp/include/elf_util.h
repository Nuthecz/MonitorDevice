//
// Created by NU on 2024/10/8.
//

#ifndef MONITORDEVICE_ELF_UTIL_H
#define MONITORDEVICE_ELF_UTIL_H

#pragma once

#include <string_view>
#include <map>
#include <linux/elf.h>
#include <sys/types.h>
#include <link.h>
#include <vector>
#include <string>
#include "../include/config.h"

#define SHT_GNU_HASH 0x6ffffff6

namespace SandHook {
    class ElfImg {
    public:

        ElfImg(std::string_view elf);

        template<typename T = void *>
        requires(std::is_pointer_v<T>)
        constexpr const T
        getSymbAddress(std::string_view
        name) const {
            auto offset = getSymbOffset(name, GnuHash(name), ElfHash(name));
            if (offset > 0 && base != nullptr) {
                return reinterpret_cast<T>(static_cast<ElfW(Addr) >
                                                       ((uintptr_t) base + offset - bias));
            } else {
                return nullptr;
            }
        }

        template<typename T = void *>
        requires(std::is_pointer_v<T>)
        constexpr const T
        getSymbPrefixFirstAddress(std::string_view
        prefix) const {
            auto offset = PrefixLookupFirst(prefix);
            if (offset > 0 && base != nullptr) {
                return reinterpret_cast<T>(static_cast<ElfW(Addr) >
                                                       ((uintptr_t) base + offset - bias));
            } else {
                return nullptr;
            }
        }

        template<typename T = void *>
        requires(std::is_pointer_v<T>)

        const std::vector <T> getAllSymbAddress(std::string_view name) const {
            auto offsets = LinearRangeLookup(name);
            std::vector <T> res;
            res.reserve(offsets.size());
            for (const auto &offset: offsets) {
                res.emplace_back(reinterpret_cast<T>(static_cast<ElfW(Addr) >
                                                                 ((uintptr_t) base + offset -
                                                                  bias)));
            }
            return res;
        }

        bool isValid() const {
            return base != nullptr;
        }

        const std::string name() const {
            return elf;
        }

        // 新增方法，用于获取 base 的值
        void *getBase() const {
            return base;
        }

        // 新定义获取段信息函数
        std::pair <uintptr_t, size_t> getTextSectionInfo() const {
            return {text_section_offset, text_section_size};
        }

        std::pair <uintptr_t, size_t> getPltSectionInfo() const {
            return {plt_section_offset, plt_section_size};
        }

        ~ElfImg();

    private:
        ElfW(Addr)
        getSymbOffset(std::string_view
        name,
        uint32_t gnu_hash, uint32_t
        elf_hash) const;

        ElfW(Addr)
        ElfLookup(std::string_view
        name,
        uint32_t hash
        ) const;

        ElfW(Addr)
        GnuLookup(std::string_view
        name,
        uint32_t hash
        ) const;

        ElfW(Addr)
        LinearLookup(std::string_view
        name) const;

        std::vector<ElfW(Addr)> LinearRangeLookup(std::string_view name) const;

        ElfW(Addr)
        PrefixLookupFirst(std::string_view
        prefix) const;

        constexpr static uint32_t
        ElfHash(std::string_view
        name);

        constexpr static uint32_t
        GnuHash(std::string_view
        name);

        bool findModuleBase();

        void MayInitLinearMap() const;

        std::string elf;
        void *base = nullptr;
        char *buffer = nullptr;
        off_t size = 0;
        off_t bias = -4396;
        ElfW(Ehdr)
        *
        header = nullptr;
        ElfW(Shdr)
        *
        section_header = nullptr;
        ElfW(Shdr)
        *
        symtab = nullptr;
        ElfW(Shdr)
        *
        strtab = nullptr;
        ElfW(Shdr)
        *
        dynsym = nullptr;
        ElfW(Sym)
        *
        symtab_start = nullptr;
        ElfW(Sym)
        *
        dynsym_start = nullptr;
        ElfW(Sym)
        *
        strtab_start = nullptr;
        ElfW(Off)
        symtab_count = 0;
        ElfW(Off)
        symstr_offset = 0;
        ElfW(Off)
        symstr_offset_for_symtab = 0;
        ElfW(Off)
        symtab_offset = 0;
        ElfW(Off)
        dynsym_offset = 0;
        ElfW(Off)
        symtab_size = 0;

        uint32_t nbucket_{};
        uint32_t *bucket_ = nullptr;
        uint32_t *chain_ = nullptr;

        uint32_t gnu_nbucket_{};
        uint32_t gnu_symndx_{};
        uint32_t gnu_bloom_size_;
        uint32_t gnu_shift2_;
        uintptr_t *gnu_bloom_filter_;
        uint32_t *gnu_bucket_;
        uint32_t *gnu_chain_;

        // 新定义段信息
        uintptr_t text_section_offset = 0;
        size_t text_section_size = 0;

        uintptr_t plt_section_offset = 0;
        size_t plt_section_size = 0;

        mutable std::map<std::string_view, ElfW(Sym) * > symtabs_;
    };

    constexpr uint32_t
    ElfImg::ElfHash(std::string_view
    name) {
    uint32_t h = 0, g;
    for (
    unsigned char p
    : name) {
    h = (h << 4) + p;
    g = h & 0xf0000000;
    h ^=
    g;
    h ^= g >> 24;
}
return
h;
}

constexpr uint32_t
ElfImg::GnuHash(std::string_view
name) {
uint32_t h = 5381;
for (
unsigned char p
: name) {
h += (h << 5) +
p;
}
return
h;
}
}

#endif //MONITORDEVICE_ELF_UTIL_H
