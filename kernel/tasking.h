#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>
#include <libsystem/list.h>

#include <libkernel/message.h>
#include <libkernel/task.h>

#include "memory.h"
#include "filesystem.h"

/* --- Task data structure -------------------------------------------------- */

struct task;

typedef void (*task_entry_t)();

typedef struct
{
    lock_t lock;
    stream_t *stream;
    bool free;
} filedescriptor_t;

typedef struct
{
    uint wakeuptick;
    bool gotwakeup;
} task_wait_time_t;

typedef struct
{
    int task_handle;
    int exitvalue;
} task_wait_task_t;

typedef struct
{
    bool has_result;
    message_t result;
    int timeout;
} task_wait_respond_t;

typedef bool (*task_wait_stream_condition_t)(stream_t*);

typedef struct 
{
    stream_t* stream;
    task_wait_stream_condition_t condition;
} task_wait_stream_t;

typedef struct task
{
    int id;
    char name[TASK_NAMESIZE]; // Frendly name of the process
    task_state_t state;

    bool user;

    uint sp;
    byte stack[TASK_STACKSIZE]; // Kernel stack
    task_entry_t entry; // Our entry point
    char fpu_registers[512];

    struct
    {
        task_wait_time_t time;
        task_wait_task_t task;
        task_wait_respond_t respond;
        task_wait_stream_t stream;
    } wait;

    List *inbox;  // process main message queue
    List *subscription;

    List *shms;

    lock_t fds_lock;
    filedescriptor_t fds[TASK_FILDES_COUNT];

    lock_t cwd_lock;
    fsnode_t* cwd_node;
    path_t* cwd_path;

    page_directorie_t *pdir; // Page directorie

    int exitvalue;
} task_t;

/* -------------------------------------------------------------------------- */
/*   TASKING                                                                  */      
/* -------------------------------------------------------------------------- */

void tasking_setup(void);
task_t* task_kernel(void);

/* -------------------------------------------------------------------------- */
/*   TASKS                                                                    */      
/* -------------------------------------------------------------------------- */

void task_setup(void);

task_t *task(task_t* parent, const char* name, bool user);

void task_delete(task_t *task);

List* task_all(void);

List* task_bystate(task_state_t state);

task_t *task_getbyid(int id);

void task_get_info(task_t* this, task_info_t* info);

int task_count(void);

task_t* task_spawn(task_t* parent, const char* name, task_entry_t entry, void *arg, bool user);

task_t* task_spawn_with_argv(task_t* parent, const char* name, task_entry_t entry, const char **argv, bool user);

void task_setstate(task_t *task, task_state_t state);

void task_setentry(task_t *t, task_entry_t entry, bool user);

uint task_stack_push(task_t *t, const void *value, uint size);

void task_go(task_t *t);

typedef enum
{
    TASK_SLEEP_RESULT_WAKEUP,
    TASk_SLEEP_RESULT_TIMEOUT,
} task_sleep_result_t;

task_sleep_result_t task_sleep(task_t* this, int timeout);

int task_wakeup(task_t* task);

bool task_wait(int task_id, int *exitvalue);

bool task_wait_stream(task_t* task, stream_t* stream, task_wait_stream_condition_t condition);

bool task_cancel(task_t* task, int exitvalue);

void task_exit(int exitvalue);

void task_dump(task_t *t);

void task_panic_dump(void);

/* --- Task memory management ----------------------------------------------- */

page_directorie_t* task_switch_pdir(task_t* task, page_directorie_t* pdir);

int task_memory_map(task_t* this, uint addr, uint count);

int task_memory_unmap(task_t* this, uint addr, uint count);

uint task_memory_alloc(task_t* this, uint count);

void task_memory_free(task_t* this, uint addr, uint count);

/* --- Task current working directory --------------------------------------- */

path_t* task_cwd_resolve(task_t* this, const char* path_to_resolve);

int task_set_cwd(task_t* this, const char* new_wd);

void task_get_cwd(task_t* this, char* buffer, uint size);

/* --- Task file system access ---------------------------------------------- */

void task_filedescriptor_close_all(task_t* this);

int task_filedescriptor_alloc_and_acquire(task_t *this, stream_t *stream);

stream_t *task_filedescriptor_acquire(task_t *this, int fd_index);

int task_filedescriptor_release(task_t *this, int fd_index);

int task_filedescriptor_free_and_release(task_t *this, int fd_index);

int task_open_file(task_t* this, const char *file_path, iostream_flag_t flags);

int task_close_file(task_t* this, int fd);

int task_read_file(task_t* this, int fd, void *buffer, uint size);

int task_write_file(task_t* this, int fd, const void *buffer, uint size);

int task_call_file(task_t* this, int fd, int request, void *args);

int task_seek_file(task_t* this, int fd, int offset, iostream_whence_t whence);

int task_tell_file(task_t* this, int fd, iostream_whence_t whence);

int task_stat_file(task_t* this, int fd, iostream_stat_t *stat);

/* -------------------------------------------------------------------------- */
/*   PROCESSES                                                                */      
/* -------------------------------------------------------------------------- */

int task_exec(const char *executable_path, const char **argv);

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
    shm_physical_region_t* region;
    uint vaddr;
} shm_virtual_region_t;

void task_shared_memory_setup(void);

shm_physical_region_t* task_physical_region_get_by_id(int id);
shm_virtual_region_t* task_virtual_region_get_by_id(task_t* this, int id);

int task_shared_memory_alloc(task_t *this, int pagecount);

int task_shared_memory_acquire(task_t *this, int shm, uint *addr);

int task_shared_memory_release(task_t *this, int shm);

/* -------------------------------------------------------------------------- */
/*   MESSAGING                                                                */      
/* -------------------------------------------------------------------------- */

int task_messaging_send(task_t* this, message_t* event);

int task_messaging_broadcast(task_t* this, const char *channel, message_t* event);

int task_messaging_request(task_t* this, message_t* request, message_t* respond, int timeout);

int task_messaging_receive(task_t* this, message_t* message, bool wait);

int task_messaging_respond(task_t* this, message_t* request, message_t* respond);

int task_messaging_subscribe(task_t* this, const char* channel);

int task_messaging_unsubscribe(task_t* this, const char* channel);

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

void sheduler_setup(task_t *main_kernel_task);

void wakeup_sleeping_tasks(void);

reg32_t shedule(reg32_t sp, processor_context_t *context);

uint sheduler_get_ticks(void);

bool sheduler_is_context_switch(void);

void sheduler_yield(void);

task_t* sheduler_running(void);

int sheduler_running_id(void);

int sheduler_get_usage(int task_id);