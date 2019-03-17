/* Copyright © 2018-2019 N. Van Bossuyt.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/version.h"

char *__kernel_name = "hjert";

int __kernel_version_major = 0;
int __kernel_version_minor = 2;
int __kernel_version_patch = 0;
char *__kernel_version_codename = "wheat";

/* kernel version format major.minor.patch-codename */
char *__kernel_version_format = "%d.%d.%d-%s @ " __COMMIT__;
char *__kernel_uname_format = "%s %d.%d.%d-%s @ " __COMMIT__;