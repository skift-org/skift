#pragma once

#ifndef ARCH_PAGE_SIZE
#    define ARCH_PAGE_SIZE (4096)
#endif

#define PAGE_ALIGN(__x) ((__x) + ARCH_PAGE_SIZE - ((__x) % ARCH_PAGE_SIZE))

#define PAGE_ALIGN_UP(__x)       \
    ((__x % ARCH_PAGE_SIZE == 0) \
         ? (__x)                 \
         : (__x) + ARCH_PAGE_SIZE - ((__x) % ARCH_PAGE_SIZE))

#define PAGE_ALIGN_DOWN(__x) ((__x) - ((__x) % ARCH_PAGE_SIZE))

#define IS_PAGE_ALIGN(__x) (__x % ARCH_PAGE_SIZE == 0)
