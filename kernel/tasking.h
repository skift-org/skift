#pragma once

#include <abi/Launchpad.h>
#include <abi/Process.h>
#include <abi/Task.h>

#include <libsystem/Result.h>
#include <libsystem/utils/List.h>

#include "kernel/filesystem/Filesystem.h"
#include "kernel/memory/Memory.h"
#include "kernel/memory/MemoryObject.h"
#include "kernel/scheduling/Blocker.h"

typedef void (*TaskEntry)();

typedef struct Task
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
} Task;

void tasking_initialize(void);

Task *task_create(Task *parent, const char *name, bool user);

void task_destroy(Task *task);

typedef IterationDecision (*TaskIterateCallback)(void *target, Task *task);
void task_iterate(void *target, TaskIterateCallback callback);

Task *task_by_id(int id);

int task_count(void);

Task *task_spawn(Task *parent, const char *name, TaskEntry entry, void *arg, bool user);

Task *task_spawn_with_argv(Task *parent, const char *name, TaskEntry entry, const char **argv, bool user);

Result task_launch(Task *parent_task, Launchpad *launchpad, int *pid);

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

/* --- Task memory management ----------------------------------------------- */

PageDirectory *task_switch_pdir(Task *task, PageDirectory *pdir);

Result task_memory_map(Task *task, MemoryRange range);

Result task_memory_alloc(Task *task, size_t size, uintptr_t *out_address);

Result task_memory_free(Task *task, MemoryRange range);

/* --- Task current working directory --------------------------------------- */

Path *task_resolve_directory(Task *task, const char *buffer);

Result task_set_directory(Task *task, const char *buffer);

Result task_get_directory(Task *task, char *buffer, uint size);

/* --- Task file system access ---------------------------------------------- */

Result task_fshandle_add(Task *task, int *handle_index, FsHandle *handle);

Result task_fshandle_remove(Task *task, int handle_index);

FsHandle *task_fshandle_acquire(Task *task, int handle_index);

Result task_fshandle_release(Task *task, int handle_index);

/* -------------------------------------------------------------------------- */
/*   SHARED MEMORY                                                            */
/* -------------------------------------------------------------------------- */

typedef struct
{
    MemoryObject *object;

    uintptr_t address;
    size_t size;
} MemoryMapping;

MemoryMapping *task_memory_mapping_create(Task *task, MemoryObject *memory_object);

void task_memory_mapping_destroy(Task *task, MemoryMapping *memory_mapping);

MemoryMapping *task_memory_mapping_by_address(Task *task, uintptr_t address);

Result task_shared_memory_alloc(Task *task, size_t size, uintptr_t *out_address);

Result task_shared_memory_free(Task *task, uintptr_t address);

Result task_shared_memory_include(Task *task, int handle, uintptr_t *out_address, size_t *out_size);

Result task_shared_memory_get_handle(Task *task, uintptr_t address, int *out_handle);

/* -------------------------------------------------------------------------- */
/*   GARBAGE COLLECTOR                                                        */
/* -------------------------------------------------------------------------- */

void garbage_collector(void);
