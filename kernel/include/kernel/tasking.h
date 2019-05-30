#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>
#include <skift/list.h>

#include <hjert/message.h>
#include <hjert/task.h>

#include "kernel/memory.h"
#include "kernel/filesystem.h"

/* --- Task data structure -------------------------------------------------- */

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
} task_wait_time_t;

typedef struct
{
    int task_handle;
    int exitvalue;
} task_wait_task_t;

typedef struct
{
    message_t *message;
} task_wait_message_t;

typedef struct task
{
    int id;
    char name[TASK_NAMESIZE]; // Frendly name of the process
    task_state_t state;

    bool user;

    uint sp;
    byte stack[TASK_STACKSIZE]; // Kernel stack
    task_entry_t entry; // Our entry point

    struct
    {
        task_wait_time_t time;
        task_wait_task_t task;
        task_wait_message_t message;
    } wait;

    lock_t inbox_lock;
    list_t *inbox;  // process main message queu

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

/* -------------------------------------------------------------------------- */
/*   TASKS                                                                    */      
/* -------------------------------------------------------------------------- */

void task_setup(void);

task_t *task(task_t* parent, const char* name, bool user);

void task_delete(task_t *task);

list_t* task_bystate(task_state_t state);

task_t *task_getbyid(int id);

int task_count(void);

task_t* task_spawn(task_t* parent, const char* name, task_entry_t entry, void *arg, bool user);

task_t* task_spawn_with_argv(task_t* parent, const char* name, task_entry_t entry, const char **argv, bool user);

void task_setstate(task_t *task, task_state_t state);

void task_setentry(task_t *t, task_entry_t entry, bool user);

uint task_stack_push(task_t *t, const void *value, uint size);

void task_go(task_t *t);

void task_sleep(int time);

int task_wakeup(task_t* task);

bool task_wait(int task_id, int *exitvalue);

bool task_cancel(task_t* task, int exitvalue);

void task_exit(int exitvalue);

void task_dump(task_t *t);

void task_panic_dump(void);

/* --- Task memory management ----------------------------------------------- */

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

int task_ioctl_file(task_t* this, int fd, int request, void *args);

int task_seek_file(task_t* this, int fd, int offset, iostream_whence_t whence);

int task_tell_file(task_t* this, int fd, iostream_whence_t whence);

int task_fstat_file(task_t* this, int fd, iostream_stat_t *stat);

/* -------------------------------------------------------------------------- */
/*   PROCESSES                                                                */      
/* -------------------------------------------------------------------------- */

int task_exec(const char *executable_path, const char **argv);

/* -------------------------------------------------------------------------- */
/*   MESSAGING                                                                */      
/* -------------------------------------------------------------------------- */

#define CHANNAME_SIZE 128

typedef struct
{
    char name[CHANNAME_SIZE];
    list_t *subscribers;
} channel_t;

void messaging_setup(void);

channel_t *channel(const char *name);

void channel_delete(channel_t *channel);

channel_t *channel_get(const char *channel_name);

message_t *message(int id, const char *label, void *payload, uint size, uint flags);

void message_delete(message_t *msg);

int messaging_send_internal(task_t*  from, task_t*  to, int id, const char *name, void *payload, uint size, uint flags);

int messaging_send(task_t* to, const char *name, void *payload, uint size, uint flags);

int messaging_broadcast(const char *channel_name, const char *name, void *payload, uint size, uint flags);

message_t *messaging_receive_internal(task_t *task);

bool messaging_receive(message_t *msg, bool wait);

int messaging_payload(void *buffer, uint size);

int messaging_subscribe(const char *channel_name);

int messaging_unsubscribe(const char *channel_name);

/* -------------------------------------------------------------------------- */
/*   GARBAGE COLECTOR                                                         */      
/* -------------------------------------------------------------------------- */

void collect_and_free_task(void);

void collect_and_free_process(void);

void garbage_colector();

/* -------------------------------------------------------------------------- */
/*   SHEDULER                                                                 */      
/* -------------------------------------------------------------------------- */

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