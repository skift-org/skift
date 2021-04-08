#pragma once

#include <abi/Time.h>
#include <libsystem/Common.h>

#define SYSTEM_INFO_FIELD_SIZE 65

struct SystemInfo
{
    char kernel_name[SYSTEM_INFO_FIELD_SIZE];    // The name of the kernel (e.g. "hjert")
    char kernel_release[SYSTEM_INFO_FIELD_SIZE]; // The version of the kernel (e.g. "0.2.0")
    char kernel_build[SYSTEM_INFO_FIELD_SIZE];
    char system_name[SYSTEM_INFO_FIELD_SIZE]; // The name of the operating system (e.g. "skiftOS")
    char machine[SYSTEM_INFO_FIELD_SIZE];     // The machine name
};

struct SystemStatus
{
    ElapsedTime uptime;
    size_t total_ram;
    size_t used_ram;
    int running_tasks;
    int cpu_usage;
};
