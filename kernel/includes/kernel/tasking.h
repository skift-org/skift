#pragma once
#include "types.h"




#define TASK_MAX_COUNT 16
#define TASK_NAME_SIZE 64
#define TASK_STACK_SIZE 4096

typedef u32 esp_t;
typedef int pid_t;
typedef void (*task_entry_t)();

typedef enum
{
    TASK_RUNNING,
    TASK_DEAD, // Dead task need to free.
    TASK_FREE
} task_state_t;

typedef PACKED(struct)
{
    pid_t id;
    char name[TASK_NAME_SIZE];
    task_state_t state;

    u32 esp;
    task_entry_t entry;
    u8 stack[TASK_STACK_SIZE];
}
task_t;

void task_setup();
pid_t task_start(task_entry_t entry);
pid_t task_start_named(task_entry_t entry, string name);

esp_t task_shedule();

#define PROCESS_STACK_SIZE 4096

typedef struct
{
    int id;
    bool user;
    void * page_directorie;
    u32 esp;
    void * stack;
} process_t;
