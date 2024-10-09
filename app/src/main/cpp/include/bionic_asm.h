#pragma once

#include <asm/unistd.h> /* For system call numbers. */

#define MAX_ERRNO 4095  /* For recognizing system call error returns. */

#define __bionic_asm_custom_entry(f)
#define __bionic_asm_custom_end(f)
#define __bionic_asm_function_type @function
#define __bionic_asm_custom_note_gnu_section()

#if defined(__aarch64__)
#define __bionic_asm_align 16

#elif defined(__x86_64__)
#define __bionic_asm_align 16

#endif


#define ENTRY_NO_DWARF(f) \
    .text; \
    .globl f; \
    .balign __bionic_asm_align; \
    .type f, __bionic_asm_function_type; \
    f: \
    __bionic_asm_custom_entry(f); \

#define ENTRY(f) \
    ENTRY_NO_DWARF(f) \
    .cfi_startproc \

#define END_NO_DWARF(f) \
    .size f, .-f; \
    __bionic_asm_custom_end(f) \

#define END(f) \
    .cfi_endproc; \
    END_NO_DWARF(f) \

/* Like ENTRY, but with hidden visibility. */
#define ENTRY_PRIVATE(f) \
    ENTRY(f); \
    .hidden f \

/* Like ENTRY_NO_DWARF, but with hidden visibility. */
#define ENTRY_PRIVATE_NO_DWARF(f) \
    ENTRY_NO_DWARF(f); \
    .hidden f \

#define __BIONIC_WEAK_ASM_FOR_NATIVE_BRIDGE(f) \
    .weak f; \

#define ALIAS_SYMBOL(alias, original) \
    .globl alias; \
    .equ alias, original

#define NOTE_GNU_PROPERTY() \
    __bionic_asm_custom_note_gnu_section()