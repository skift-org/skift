#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <abi/Filesystem.h>
#include <abi/Process.h>

typedef enum
{
    TASK_STATE_NONE = -1,

    TASK_STATE_HANG,
    TASK_STATE_LAUNCHPAD,
    TASK_STATE_RUNNING,
    TASK_STATE_BLOCKED,
    TASK_STATE_WAIT_TIME,
    TASK_STATE_WAIT_TASK,
    TASK_STATE_WAIT_MESSAGE,
    TASK_STATE_WAIT_RESPOND,
    TASK_STATE_CANCELED,

    TASK_STATE_COUNT
} TaskState;

typedef struct
{
    int id;
    TaskState state;

    char name[PROCESS_NAME_SIZE];
    char cwd[PATH_LENGHT];

    int usage_cpu;
    int usage_virtual_memory;
    int usage_physical_memory;
} TaskInfo;