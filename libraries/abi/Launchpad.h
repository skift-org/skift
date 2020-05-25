#pragma once

#include <abi/Filesystem.h>
#include <abi/Process.h>

typedef struct
{
    char name[PROCESS_NAME_SIZE];
    char executable[PATH_LENGTH];

    char *argv[PROCESS_ARG_COUNT + 1];
    int argc;

    int handles[PROCESS_HANDLE_COUNT];
} Launchpad;
