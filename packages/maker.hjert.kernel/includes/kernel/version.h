#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>

#define KERNEL_VERSION __kernel_version_format, __kernel_version_major, __kernel_version_minor, __kernel_version_patch, __kernel_version_codename
#define KERNEL_UNAME __kernel_uname_format, __kernel_name, __kernel_version_major, __kernel_version_minor, __kernel_version_patch, __kernel_version_codename

extern char *__kernel_name;
extern int __kernel_version_major;
extern int __kernel_version_minor;
extern int __kernel_version_patch;
extern char *__kernel_version_codename;

extern char *__kernel_version_format;
extern char *__kernel_uname_format;