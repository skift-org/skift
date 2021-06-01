#pragma once

#include <abi/Process.h>
#include <abi/Task.h>

#include <libio/Path.h>
#include <libutils/List.h>

#include "system/memory/Memory.h"
#include "system/scheduling/Blocker.h"

#include "system/tasking/Domain.h"
#include "system/tasking/Handles.h"

typedef void (*TaskEntryPoint)();

struct MemoryMapping;

struct Task
{
    int id;
    char name[PROCESS_NAME_SIZE];
    TaskFlags _flags;

    Syscall _current_syscall;
    bool _is_doing_syscall = false;
    bool _is_canceled = false;
    bool _is_interrupted = false;

    TaskState _state;
    Blocker *_blocker;

    uintptr_t user_stack_pointer;
    void *user_stack;

    uintptr_t kernel_stack_pointer;
    void *kernel_stack;

    TaskEntryPoint entry_point;
    char fpu_registers[512];

    List<MemoryMapping *> *memory_mapping;
    Arch::AddressSpace *address_space;

    int exit_value = 0;

    Handles _handles;
    Domain _domain;

    Handles &handles() { return _handles; }
    Domain &domain() { return _domain; }

    TaskState state();

    void state(TaskState state);

    HjResult cancel(int exit_value);

    void try_unblock()
    {
        if (_blocker->can_unblock(*this))
        {
            _blocker->unblock(*this);
            state(TASK_STATE_RUNNING);
        }
        else if (_blocker->has_timeout())
        {
            _blocker->timeout(*this);
            state(TASK_STATE_RUNNING);
        }
        else if (_blocker->is_interrupted())
        {
            state(TASK_STATE_RUNNING);
        }
    }

    void begin_syscall(Syscall current)
    {
        _is_doing_syscall = true;
        _current_syscall = current;
    }

    void end_syscall()
    {
        _current_syscall = (Syscall)-1;
        _is_doing_syscall = false;
    }

    void interrupt();

    void kill_me_if_you_dare();
};

Task *task_create(Task *parent, const char *name, TaskFlags flags);

Task *task_clone(Task *parent, uintptr_t sp, uintptr_t ip, TaskFlags flags);

void task_destroy(Task *task);

void task_clear_userspace(Task *task);

typedef Iteration (*TaskIterateCallback)(void *target, Task *task);
void task_iterate(void *target, TaskIterateCallback callback);

Task *task_by_id(int id);

int task_count();

Task *task_spawn(Task *parent, const char *name, TaskEntryPoint entry, void *arg, TaskFlags flags);

void task_set_entry(Task *task, TaskEntryPoint entry);

void task_pass_argc_argv_env(Task *task, const char **argv, const char *env);

uintptr_t task_kernel_stack_push(Task *task, const void *value, size_t size);

uintptr_t task_user_stack_push(Task *task, const void *value, size_t size);

uintptr_t task_user_stack_push_long(Task *task, long value);

uintptr_t task_user_stack_push_ptr(Task *task, void *ptr);

void task_go(Task *task);

HjResult task_sleep(Task *task, int timeout);

HjResult task_wait(int task_id, int *exit_value);

HjResult task_block(Task *task, Blocker &blocker, Timeout timeout);

void task_dump(Task *task);
