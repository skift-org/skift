#pragma once

#include <skift/runtime.h>

#define SYSTEM_INFO_FIELD_SIZE 65

typedef struct
{
    char kernel_name[SYSTEM_INFO_FIELD_SIZE];    // The name of the kernel (e.g. "hjert")
    char kernel_release[SYSTEM_INFO_FIELD_SIZE]; // The version of the operating (e.g. "0.2.0")
    char system_name[SYSTEM_INFO_FIELD_SIZE];    // The name of the operating system (e.g. "skiftOS")

    char machine[SYSTEM_INFO_FIELD_SIZE]; // The machine name
} system_info_t;

typedef struct
{
    uint64_t uptime;

    uint total_ram;
    uint used_ram;

    int running_process;
    int running_threads;
} system_status_t;

// Retrived system information like kernel name and system release.
void system_get_info(system_info_t *info);
void system_get_status(system_status_t * status);