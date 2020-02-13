#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <abi/Filesystem.h>
#include <abi/Process.h>

typedef struct
{
    char name[PROCESS_NAME_SIZE];
    char executable[PATH_LENGHT];

    char *argv[PROCESS_ARG_COUNT + 1];
    int argc;

    int handles[PROCESS_HANDLE_COUNT];
} Launchpad;
