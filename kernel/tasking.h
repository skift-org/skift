#pragma once

#include <abi/Launchpad.h>
#include <abi/Process.h>
#include <abi/Task.h>

#include <libsystem/Result.h>
#include <libsystem/utils/List.h>

#include "kernel/filesystem/Filesystem.h"
#include "kernel/memory/Memory.h"
#include "kernel/sheduling/Blocker.h"

struct Task;

typedef void (*TaskEntry)();

typedef struct Task
{
    int id;
    bool user;
    char name[PROCESS_NAME_SIZE]; // Friendly name of the process

    TaskState state;
    Blocker *blocker;

    uintptr_t stack_pointer;
    byte *stack;     // Kernel stack
    TaskEntry entry; // Our entry point
    char fpu_registers[512];

    Lock handles_lock;
    FsHandle *handles[PROCESS_HANDLE_COUNT];

    Lock cwd_lock;
    Path *cwd_path;

    List *memory_mapping;
    PageDirectory *pdir; // Page directory

    int exit_value;
} Task;

void tasking_initialize(void);

Task *task_create(Task *parent, const char *name, bool user);

void task_destroy(Task *task);

List *task_all(void);

List *task_by_state(TaskState state);

Task *task_by_id(int id);

int task_count(void);

Task *task_spawn(Task *parent, const char *name, TaskEntry entry, void *arg, bool user);

Task *task_spawn_with_argv(Task *parent, const char *name, TaskEntry entry, const char **argv, bool user);

Result task_launch(Task *parent_task, Launchpad *launchpad, int *pid);

void task_set_state(Task *task, TaskState state);

void task_set_entry(Task *task, TaskEntry entry, bool user);

uintptr_t task_stack_push(Task *task, const void *value, uint size);

void task_go(Task *t);

Result task_sleep(Task *task, int timeout);

Result task_wait(int task_id, int *exit_value);

BlockerResult task_block(Task *task, Blocker *blocker, Timeout timeout);

Result task_cancel(Task *task, int exit_value);

void task_exit(int exit_value);

void task_dump(Task *t);

void task_panic_dump(void);

/* --- Task memory management ----------------------------------------------- */

PageDirectory *task_switch_pdir(Task *task, PageDirectory *pdir);

int task_memory_map(Task *task, uint addr, uint count);

int task_memory_unmap(Task *task, uint addr, uint count);

uint task_memory_alloc(Task *task, uint count);

void task_memory_free(Task *task, uint addr, uint count);

/* --- Task current working directory --------------------------------------- */

Path *task_cwd_resolve(Task *task, const char *buffer);

Result task_set_cwd(Task *task, const char *buffer);

void task_get_cwd(Task *task, char *buffer, uint size);

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
    int id;
    uintptr_t address;
    size_t size;

    int refcount;
} MemoryObject;

typedef struct
{
    MemoryObject *object;

    uintptr_t address;
    size_t size;
} MemoryMapping;

void task_shared_memory_setup(void);

MemoryObject *memory_object_create(size_t size);

void memory_object_destroy(MemoryObject *memory_object);

MemoryObject *memory_object_ref(MemoryObject *memory_object);

void memory_object_deref(MemoryObject *memory_object);

MemoryObject *memory_object_by_id(int id);

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

void collect_and_free_task(void);

void collect_and_free_process(void);

void garbage_collector();

/* -------------------------------------------------------------------------- */
/*   SCHEDULER                                                                */
/* -------------------------------------------------------------------------- */

#define SCHEDULER_RECORD_COUNT 1000

uintptr_t schedule(uintptr_t current_stack_pointer);

bool scheduler_is_context_switch(void);

void scheduler_yield(void);

Task *scheduler_running(void);

int scheduler_running_id(void);

int scheduler_get_usage(int task_id);
