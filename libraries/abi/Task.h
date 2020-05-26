#pragma once

#include <abi/Filesystem.h>
#include <abi/Process.h>

typedef enum
{
    TASK_STATE_NONE = -1,

    TASK_STATE_HANG,
    TASK_STATE_LAUNCHPAD,
    TASK_STATE_RUNNING,
    TASK_STATE_BLOCKED,
    TASK_STATE_WAIT,
    TASK_STATE_CANCELED,

    __TASK_STATE_COUNT
} TaskState;

typedef struct
{
    int id;
    TaskState state;

    char name[PROCESS_NAME_SIZE];
    char cwd[PATH_LENGTH];

    int usage_cpu;
    int usage_virtual_memory;
    int usage_physical_memory;
} TaskInfo;
