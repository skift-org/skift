#pragma once

#include <abi/Process.h>
#include <abi/Task.h>

#include <libsystem/utils/List.h>
#include <libutils/Path.h>

#include "kernel/memory/Memory.h"
#include "kernel/scheduling/Blocker.h"

#include "kernel/tasking/Domain.h"
#include "kernel/tasking/Handles.h"

typedef void (*TaskEntryPoint)();

struct Task
{
    int id;
    bool user;
    char name[PROCESS_NAME_SIZE];

    TaskState _state;
    Blocker *blocker;

    uintptr_t user_stack_pointer;
    void *user_stack;

    uintptr_t kernel_stack_pointer;
    void *kernel_stack;

    TaskEntryPoint entry_point;
    char fpu_registers[512];

    List *memory_mapping;
    void *address_space;

    int exit_value = 0;

    Handles _handles;
    Domain _domain;

    Handles &handles() { return _handles; }
    Domain &domain() { return _domain; }

    TaskState state();

    void state(TaskState state);

    void cancel(int exit_value);
};

Task *task_create(Task *parent, const char *name, bool user);

Task *task_clone(Task *parent, uintptr_t sp, uintptr_t ip);

void task_destroy(Task *task);

void task_clear_userspace(Task *task);

typedef Iteration (*TaskIterateCallback)(void *target, Task *task);
void task_iterate(void *target, TaskIterateCallback callback);

Task *task_by_id(int id);

int task_count();

Task *task_spawn(Task *parent, const char *name, TaskEntryPoint entry, void *arg, bool user);

void task_set_entry(Task *task, TaskEntryPoint entry, bool user);

void task_pass_argc_argv_env(Task *task, const char **argv, const char *env);

uintptr_t task_kernel_stack_push(Task *task, const void *value, size_t size);

uintptr_t task_user_stack_push(Task *task, const void *value, size_t size);

uintptr_t task_user_stack_push_long(Task *task, long value);

uintptr_t task_user_stack_push_ptr(Task *task, void *ptr);

void task_go(Task *task);

Result task_sleep(Task *task, int timeout);

Result task_wait(int task_id, int *exit_value);

BlockerResult task_block(Task *task, Blocker *blocker, Timeout timeout);

void task_dump(Task *task);
