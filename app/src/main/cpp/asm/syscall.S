#include "bionic_asm.h"

#if defined(__aarch64__)

ENTRY(my_openat_svc)
    mov x8, __NR_openat
    svc     #0
    cmn x0, #(MAX_ERRNO + 1)
    cneg x0, x0, hi
    //    b.hi    __set_errno_internal
    ret
END(my_openat_svc)


#elif defined(__x86_64__)

ENTRY(my_openat_svc)
    movl    $__NR_openat, %eax
    syscall
    cmpq    $-MAX_ERRNO, %rax
    jb      my_openat_return
    negl    %eax
    movl    %eax, %edi
//    call    __set_errno_internal
my_openat_return:
    ret
END(my_openat_svc)


#endif