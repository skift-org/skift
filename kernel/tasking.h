#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libkernel/message.h>
#include <libsystem/error.h>
#include <libsystem/list.h>
#include <libsystem/runtime.h>

#include <abi/Launchpad.h>
#include <abi/Process.h>
#include <abi/Task.h>

#include "filesystem/Filesystem.h"
#include "memory.h"
#include "sheduling/TaskBlocker.h"

/* --- Task data structure -------------------------------------------------- */

struct Task;

typedef void (*TaskEntry)();

typedef struct
{
    uint wakeuptick;
    bool gotwakeup;
} TaskWaitTimeInfo;

typedef struct
{
    int task_handle;
    int exitvalue;
} TaskWaitTaskInfo;

typedef struct
{
    bool has_result;
    message_t result;
    int timeout;
} TaskWaitRespondInfo;

typedef struct Task
{
    int id;
    char name[PROCESS_NAME_SIZE]; // Frendly name of the process
    TaskState state;

    bool user;

    uint sp;
    byte stack[PROCESS_STACK_SIZE]; // Kernel stack
    TaskEntry entry;                // Our entry point
    char fpu_registers[512];

    struct
    {
        TaskWaitTimeInfo time;
        TaskWaitTaskInfo task;
        TaskWaitRespondInfo respond;
    } wait;

    TaskBlocker *blocker;

    List *inbox; // process main message queue
    List *subscription;

    List *shms;

    Lock handles_lock;
    FsHandle *handles[PROCESS_HANDLE_COUNT];

    Lock cwd_lock;
    Path *cwd_path;

    page_directorie_t *pdir; // Page directorie

    int exitvalue;
} Task;

/* -------------------------------------------------------------------------- */
/*   TASKING                                                                  */
/* -------------------------------------------------------------------------- */

void tasking_setup(void);

Task *task_kernel(void);

/* -------------------------------------------------------------------------- */
/*   TASKS                                                                    */
/* -------------------------------------------------------------------------- */

void task_setup(void);

Task *task_create(Task *parent, const char *name, bool user);

void task_destroy(Task *task);

List *task_all(void);

List *task_bystate(TaskState state);

Task *task_getbyid(int id);

void task_get_info(Task *task, TaskInfo *info);

int task_count(void);

Task *task_spawn(Task *parent, const char *name, TaskEntry entry, void *arg, bool user);

Task *task_spawn_with_argv(Task *parent, const char *name, TaskEntry entry, const char **argv, bool user);

int task_launch(Task *task, Launchpad *Launchpad);

void task_setstate(Task *task, TaskState state);

void task_setentry(Task *task, TaskEntry entry, bool user);

uint task_stack_push(Task *task, const void *value, uint size);

void task_go(Task *t);

typedef enum
{
    TASK_SLEEP_RESULT_WAKEUP,
    TASk_SLEEP_RESULT_TIMEOUT,
} task_sleep_result_t;

task_sleep_result_t task_sleep(Task *this, int timeout);

int task_wakeup(Task *task);

bool task_wait(int task_id, int *exitvalue);

void task_block(Task *task, TaskBlocker *blocker);

bool task_cancel(Task *task, int exitvalue);

void task_exit(int exitvalue);

void task_dump(Task *t);

void task_panic_dump(void);

/* --- Task memory management ----------------------------------------------- */

page_directorie_t *task_switch_pdir(Task *task, page_directorie_t *pdir);

int task_memory_map(Task *this, uint addr, uint count);

int task_memory_unmap(Task *this, uint addr, uint count);

uint task_memory_alloc(Task *this, uint count);

void task_memory_free(Task *this, uint addr, uint count);

/* --- Task current working directory --------------------------------------- */

Path *task_cwd_resolve(Task *this, const char *Patho_resolve);

int task_set_cwd(Task *this, const char *new_wd);

void task_get_cwd(Task *this, char *buffer, uint size);

/* --- Task file system access ---------------------------------------------- */

error_t task_fshandle_add(Task *task, int *handle_index, FsHandle *handle);

error_t task_fshandle_remove(Task *task, int handle_index);

FsHandle *task_fshandle_acquire(Task *task, int handle_index);

error_t task_fshandle_release(Task *task, int handle_index);

/* --- Task handle operations ----------------------------------------------- */

error_t task_fshandle_open(Task *this, int *handle_index, const char *path, OpenFlag flags);

error_t task_fshandle_close(Task *this, int handle_index);

void task_fshandle_close_all(Task *this);

error_t task_fshandle_select(Task *this, int *handle_indices, SelectEvent *events, size_t count, int *selected_index);

error_t task_fshandle_read(Task *this, int handle_index, void *buffer, size_t size, size_t *readed);

error_t task_fshandle_write(Task *this, int handle_index, const void *buffer, size_t size, size_t *written);

error_t task_fshandle_seek(Task *this, int handle_index, int offset, Whence whence);

error_t task_fshandle_tell(Task *this, int handle_index, Whence whence, int *offset);

error_t task_fshandle_call(Task *this, int handle_index, int request, void *args);

error_t task_fshandle_stat(Task *this, int handle_index, FileState *stat);

error_t task_fshandle_connect(Task *this, int *handle_index, const char *path);

error_t task_fshandle_accept(Task *task, int handle_index, int *connection_handle_index);

error_t task_fshandle_send(Task *this, int handle_index, Message *message);

error_t task_fshandle_receive(Task *this, int handle_index, Message *message);

error_t task_fshandle_payload(Task *this, int handle_index, Message *message);

error_t task_fshandle_discard(Task *this, int handle_index);

error_t task_create_pipe(Task *task, int *reader_handle_index, int *writer_handle_index);

error_t task_create_term(Task *task, int *master_handle_index, int *slave_handle_index);

/* -------------------------------------------------------------------------- */
/*   SHARED MEMORY                                                            */
/* -------------------------------------------------------------------------- */

typedef struct
{
    int ID;
    uint paddr;
    int pagecount;
} shm_physical_region_t;

typedef struct
{
    shm_physical_region_t *region;
    uint vaddr;
} shm_virtual_region_t;

void task_shared_memory_setup(void);

shm_physical_region_t *task_physical_region_get_by_id(int id);
shm_virtual_region_t *task_virtual_region_get_by_id(Task *this, int id);

int task_shared_memory_alloc(Task *this, int pagecount);

int task_shared_memory_acquire(Task *this, int shm, uint *addr);

int task_shared_memory_release(Task *this, int shm);

/* -------------------------------------------------------------------------- */
/*   MESSAGING                                                                */
/* -------------------------------------------------------------------------- */

int task_messaging_send(Task *this, message_t *event);

int task_messaging_broadcast(Task *this, const char *channel, message_t *event);

int task_messaging_request(Task *this, message_t *request, message_t *respond, int timeout);

int task_messaging_receive(Task *this, message_t *message, bool wait);

int task_messaging_respond(Task *this, message_t *request, message_t *respond);

int task_messaging_subscribe(Task *this, const char *channel);

int task_messaging_unsubscribe(Task *this, const char *channel);

/* -------------------------------------------------------------------------- */
/*   GARBAGE COLECTOR                                                         */
/* -------------------------------------------------------------------------- */

void collect_and_free_task(void);

void collect_and_free_process(void);

void garbage_colector();

/* -------------------------------------------------------------------------- */
/*   SHEDULER                                                                 */
/* -------------------------------------------------------------------------- */

#define SHEDULER_RECORD_COUNT 128

void timer_set_frequency(int hz);

void sheduler_setup(Task *main_kernel_task);

void wakeup_sleeping_tasks(void);

reg32_t shedule(reg32_t sp, processor_context_t *context);

uint sheduler_get_ticks(void);

bool sheduler_is_context_switch(void);

void sheduler_yield(void);

Task *sheduler_running(void);

int sheduler_running_id(void);

int sheduler_get_usage(int task_id);
