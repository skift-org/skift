#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>
#include <skift/list.h>

#include <hjert/shared/message.h>
#include <hjert/shared/task.h>

#include "kernel/memory.h"
#include "kernel/filesystem.h"

/* --- Thread data structure ------------------------------------------------ */

typedef int THREAD; // Thread handle

typedef void (*thread_entry_t)();

typedef struct
{
    lock_t lock;
    stream_t *stream;
    bool free;
} filedescriptor_t;

typedef struct
{
    uint wakeuptick;
} thread_wait_time_t;


typedef struct
{
    int thread_handle;
    int exitvalue;
} thread_wait_thread_t;

typedef struct
{
    message_t *message;
} thread_wait_message_t;

typedef struct thread
{
    int id;
    char name[TASK_NAMESIZE]; // Frendly name of the process
    task_state_t state;

    bool user;

    uint sp;
    byte stack[TASK_STACKSIZE]; // Kernel stack
    thread_entry_t entry; // Our entry point

    struct
    {
        thread_wait_time_t time;
        thread_wait_thread_t thread;
        thread_wait_message_t message;
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
} thread_t;

/* -------------------------------------------------------------------------- */
/*   TASKING                                                                  */      
/* -------------------------------------------------------------------------- */

void tasking_setup(void);

/* -------------------------------------------------------------------------- */
/*   THREADS                                                                  */      
/* -------------------------------------------------------------------------- */

void thread_setup(void);

thread_t *thread(thread_t* parent, const char* name, bool user);

void thread_delete(thread_t *thread);

list_t* thread_bystate(task_state_t state);

thread_t *thread_getbyid(int id);

int thread_count(void);

thread_t* thread_spawn(thread_t* parent, const char* name, thread_entry_t entry, void *arg, bool user);

thread_t* thread_spawn_with_argv(thread_t* parent, const char* name, thread_entry_t entry, const char **argv, bool user);

void thread_setstate(thread_t *thread, task_state_t state);

void thread_setentry(thread_t *t, thread_entry_t entry, bool user);

uint thread_stack_push(thread_t *t, const void *value, uint size);

void thread_go(thread_t *t);

void thread_sleep(int time);

int thread_wakeup(thread_t* thread);

bool thread_wait(int thread_id, int *exitvalue);

bool thread_cancel(thread_t* thread, int exitvalue);

void thread_exit(int exitvalue);

void thread_dump(thread_t *t);

void thread_panic_dump(void);

/* --- Thread memory management --------------------------------------------- */

int thread_memory_map(thread_t* this, uint addr, uint count);

int thread_memory_unmap(thread_t* this, uint addr, uint count);

uint thread_memory_alloc(thread_t* this, uint count);

void thread_memory_free(thread_t* this, uint addr, uint count);

/* --- Thread current working directory ------------------------------------- */

path_t* thread_cwd_resolve(thread_t* this, const char* path_to_resolve);

bool thread_set_cwd(thread_t* this, const char* new_wd);

void thread_get_cwd(thread_t* this, char* buffer, uint size);

/* --- Thread file system access -------------------------------------------- */

void thread_filedescriptor_close_all(thread_t* this);

int thread_filedescriptor_alloc_and_acquire(thread_t *this, stream_t *stream);

stream_t *thread_filedescriptor_acquire(thread_t *this, int fd_index);

int thread_filedescriptor_release(thread_t *this, int fd_index);

int thread_filedescriptor_free_and_release(thread_t *this, int fd_index);

int thread_open_file(thread_t* this, const char *file_path, iostream_flag_t flags);

int thread_close_file(thread_t* this, int fd);

int thread_read_file(thread_t* this, int fd, void *buffer, uint size);

int thread_write_file(thread_t* this, int fd, const void *buffer, uint size);

int thread_ioctl_file(thread_t* this, int fd, int request, void *args);

int thread_seek_file(thread_t* this, int fd, int offset, iostream_whence_t whence);

int thread_tell_file(thread_t* this, int fd, iostream_whence_t whence);

int thread_fstat_file(thread_t* this, int fd, iostream_stat_t *stat);

/* -------------------------------------------------------------------------- */
/*   PROCESSES                                                                */      
/* -------------------------------------------------------------------------- */

int thread_exec(const char *executable_path, const char **argv);

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

int messaging_send_internal(thread_t*  from, thread_t*  to, int id, const char *name, void *payload, uint size, uint flags);

int messaging_send(thread_t* to, const char *name, void *payload, uint size, uint flags);

int messaging_broadcast(const char *channel_name, const char *name, void *payload, uint size, uint flags);

message_t *messaging_receive_internal(thread_t *thread);

bool messaging_receive(message_t *msg, bool wait);

int messaging_payload(void *buffer, uint size);

int messaging_subscribe(const char *channel_name);

int messaging_unsubscribe(const char *channel_name);

/* -------------------------------------------------------------------------- */
/*   GARBAGE COLECTOR                                                         */      
/* -------------------------------------------------------------------------- */

void collect_and_free_thread(void);

void collect_and_free_process(void);

void garbage_colector();

/* -------------------------------------------------------------------------- */
/*   SHEDULER                                                                 */      
/* -------------------------------------------------------------------------- */

void timer_set_frequency(int hz);

void sheduler_setup(thread_t *main_kernel_thread);

void wakeup_sleeping_threads(void);

reg32_t shedule(reg32_t sp, processor_context_t *context);

uint sheduler_get_ticks(void);

bool sheduler_is_context_switch(void);

void sheduler_yield(void);

thread_t* sheduler_running(void);

int sheduler_running_id(void);
