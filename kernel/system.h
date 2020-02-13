#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/logger.h>
#include <libsystem/runtime.h>

#include "kernel/x86/Interrupts.h"
#include "kernel/x86/x86.h"

void __attribute__((noreturn)) __panic(const char *file, const char *function, const int line, InterruptStackFrame *stackframe, const char *message, ...);

#define HANG   \
    while (1)  \
    {          \
        hlt(); \
    }

#define STOP   \
    while (1)  \
    {          \
        cli(); \
        hlt(); \
    }

#define PANIC(x...) __panic(__FILE__, __FUNCTION__, __LINE__, NULL, x)

#define CPANIC(ctx, x...) __panic(__FILE__, __FUNCTION__, __LINE__, ctx, x)

#define setup(x, arg...)                     \
    {                                        \
        logger_info("Setting up " #x "..."); \
        x##_setup(arg);                      \
    }

#define KERNEL_VERSION __kernel_version_format, __kernel_version_major, __kernel_version_minor, __kernel_version_patch, __kernel_version_codename

#define KERNEL_UNAME __kernel_uname_format, __kernel_name, __kernel_version_major, __kernel_version_minor, __kernel_version_patch, __kernel_version_codename

extern char *__kernel_name;
extern int __kernel_version_major;
extern int __kernel_version_minor;
extern int __kernel_version_patch;
extern char *__kernel_version_codename;

extern char *__kernel_version_format;
extern char *__kernel_uname_format;
