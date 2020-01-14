#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libkernel/filesystem.h>

#define TASK_NAMESIZE 128
#define TASK_STACKSIZE 16384
#define TASK_ARGV_COUNT 32
#define TASK_FILDES_COUNT 32

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
} task_state_t;

typedef struct
{
    int id;
    task_state_t state;

    char name[TASK_NAMESIZE];
    char cwd[PATH_LENGHT];

    int usage_cpu;
    int usage_virtual_memory;
    int usage_physical_memory;
} task_info_t;