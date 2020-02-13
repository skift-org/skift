#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

#define SYSTEM_INFO_FIELD_SIZE 65

typedef struct
{
    char kernel_name[SYSTEM_INFO_FIELD_SIZE];    // The name of the kernel (e.g. "hjert")
    char kernel_release[SYSTEM_INFO_FIELD_SIZE]; // The version of the kernel (e.g. "0.2.0")
    char system_name[SYSTEM_INFO_FIELD_SIZE];    // The name of the operating system (e.g. "skiftOS")
    char machine[SYSTEM_INFO_FIELD_SIZE];        // The machine name
} SystemInfo;

typedef struct
{
    uint64_t uptime;
    size_t total_ram;
    size_t used_ram;
    int running_tasks;
} SystemStatus;
