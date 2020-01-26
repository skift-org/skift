#pragma once

#include "abi/Filesystem.h"
#include "abi/Process.h"

typedef struct
{
    char name[PROCESS_NAME_SIZE];
    char executable[PATH_LENGHT];

    char *argv[PROCESS_ARG_COUNT + 1];
    int argc;
} Launchpad;
