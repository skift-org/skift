#pragma once

#include <abi/Filesystem.h>
#include <abi/Process.h>
#include <abi/Task.h>

struct LaunchpadArgument
{
    char *buffer;
    size_t size;
};

struct Launchpad
{
    TaskFlags flags;

    char name[PROCESS_NAME_SIZE];
    char executable[PATH_LENGTH];

    LaunchpadArgument argv[PROCESS_ARG_COUNT + 1];
    int argc;

    char *env;
    size_t env_size;

    int handles[PROCESS_HANDLE_COUNT];
};
