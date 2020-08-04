#pragma once

#include <abi/Process.h>
#include <abi/Task.h>

#include <libsystem/utils/List.h>

#include "kernel/memory/Memory.h"
#include "kernel/scheduling/Blocker.h"

typedef void (*TaskEntry)();

struct Task
{
    int id;
    bool user;
    char name[PROCESS_NAME_SIZE]; // Friendly name of the process

    TaskState state;
    Blocker *blocker;

    uintptr_t stack_pointer;
    void *stack;     // Kernel stack
    TaskEntry entry; // Our entry point
    char fpu_registers[512];

    Lock handles_lock;
    FsHandle *handles[PROCESS_HANDLE_COUNT];

    Lock directory_lock;
    Path *directory;

    List *memory_mapping;
    PageDirectory *pdir; // Page directory

    int exit_value;
};

Task *task_create(Task *parent, const char *name, bool user);

void task_destroy(Task *task);

typedef IterationDecision (*TaskIterateCallback)(void *target, Task *task);
void task_iterate(void *target, TaskIterateCallback callback);

Task *task_by_id(int id);

int task_count();

Task *task_spawn(Task *parent, const char *name, TaskEntry entry, void *arg, bool user);

Task *task_spawn_with_argv(Task *parent, const char *name, TaskEntry entry, const char **argv, bool user);

void task_set_state(Task *task, TaskState state);

void task_set_entry(Task *task, TaskEntry entry, bool user);

uintptr_t task_stack_push(Task *task, const void *value, uint size);

void task_go(Task *task);

Result task_sleep(Task *task, int timeout);

Result task_wait(int task_id, int *exit_value);

BlockerResult task_block(Task *task, Blocker *blocker, Timeout timeout);

Result task_cancel(Task *task, int exit_value);

void task_exit(int exit_value);

void task_dump(Task *task);

