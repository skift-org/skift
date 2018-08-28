/* Copyright (c) 2018 Nicolas Van Bossuyt.                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/version.h"

char * __kernel_name = "skiftOS";

int    __kernel_version_major = 0;
int    __kernel_version_minor = 1;
int    __kernel_version_patch = 0;
char * __kernel_version_codename = "babystep";

/* kernel version format major.minor.patch-codename */
char * __kernel_version_format = "%d.%d.%d-%s";
char * __kernel_uname_format = "%s %d.%d.%d-%s";