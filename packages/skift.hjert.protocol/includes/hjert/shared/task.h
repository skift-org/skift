#pragma once

#include <hjert/shared/filesystem.h>

#define TASK_NAMESIZE 128 
#define TASK_STACKSIZE 16384
#define TASK_ARGV_COUNT 32
#define TASK_FILDES_COUNT 32

typedef enum
{
    TASK_STATE_NONE = -1,
    
    TASK_STATE_HANG,
    TASK_STATE_RUNNING,
    TASK_STATE_WAIT_TIME,
    TASK_STATE_WAIT_THREAD,
    TASK_STATE_WAIT_MESSAGE,
    TASK_STATE_CANCELED,

    TASK_STATE_COUNT
} task_state_t;

typedef struct
{
    int id;
    task_state_t state;
    
    const char name[TASK_NAMESIZE];
    const char cwd[PATH_LENGHT];

    int usage_cpu;
    int usage_virtual_memory;
    int usage_physical_memory;
} task_info_t;