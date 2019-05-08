#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>
#include <skift/list.h>

#include "kernel/memory.h"
#include "kernel/filesystem.h"
#include "kernel/protocol.h"
#include "kernel/limits.h"

/* --- Process data structure ----------------------------------------------- */

typedef int PROCESS; // Process handler

typedef enum e_process_state
{
    PROCESS_LAUNCHPAD,

    PROCESS_RUNNING,

    // This is like a *zombie* process in **UNIX**.
    PROCESS_CANCELING,

    PROCESS_CANCELED, // This process is ready to be garbage colected.
} process_state_t;

typedef struct
{
    lock_t lock;
    stream_t *stream;
    bool free;
} process_filedescriptor_t;

typedef struct s_process
{
    int id;                          // Unique handle to the process
    bool user;                       // Is this a user process
    char name[MAX_PROCESS_NAMESIZE]; // Frendly name of the process
    struct s_process *parent;        // Our parent

    list_t *threads;   // Child threads
    list_t *processes; // Child processes

    list_t *inbox;  // process main message queu

    lock_t fds_lock;
    process_filedescriptor_t fds[MAX_PROCESS_OPENED_FILES];

    page_directorie_t *pdir; // Page directorie
    process_state_t state;   // State of the process (RUNNING, CANCELED)

    int exitvalue;
} process_t;

/* --- Thread data structure ------------------------------------------------ */

typedef int THREAD; // Thread handle
typedef void (*thread_entry_t)();

typedef enum thread_state
{
    THREADSTATE_NONE = -1,

    THREADSTATE_RUNNING,
    THREADSTATE_WAIT_TIME,
    THREADSTATE_WAIT_THREAD,
    THREADSTATE_WAIT_PROCESS,
    THREADSTATE_WAIT_MESSAGE,
    THREADSTATE_CANCELED,

    THREADSTATE_COUNT
} thread_state_t;

typedef struct
{
    uint wakeuptick;
} thread_wait_time_t;

typedef struct
{
    int process_handle;
    int exitvalue;
} thread_wait_process_t;

typedef struct
{
    int thread_handle;
    int exitvalue;
} thread_wait_thread_t;

typedef struct
{
    message_t *message;
} thread_wait_message_t;

typedef struct
{
    int id;
    struct s_process *process;   // The parent process
    thread_entry_t entry; // Our entry point

    thread_state_t state;

    bool user;
    uint sp;
    byte stack[MAX_THREAD_STACKSIZE]; // Kernel stack

    struct
    {
        thread_wait_time_t time;
        thread_wait_process_t process;
        thread_wait_thread_t thread;
        thread_wait_message_t message;
    } wait;

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

thread_t *thread(void);

void thread_delete(thread_t *thread);

list_t* thread_bystate(thread_state_t state);

thread_t *thread_getbyid(int id);

int thread_count(void);

THREAD thread_create_mainthread(process_t* parent_process, thread_entry_t entry, const char **argv);

THREAD thread_create(process_t* parent_process, thread_entry_t entry, void *arg, bool user);

void thread_setstate(thread_t *thread, thread_state_t state);

void thread_setentry(thread_t *t, thread_entry_t entry, bool user);

uint thread_stack_push(thread_t *t, const void *value, uint size);

void thread_attach_to_process(thread_t *t, process_t *p);

void thread_setready(thread_t *t);

void thread_sleep(int time);

void thread_wakeup(THREAD t);

bool thread_wait_thread(THREAD t, int *exitvalue);

bool thread_wait_process(PROCESS p, int *exitvalue);

bool thread_cancel(THREAD t, int exitvalue);

void thread_exit(int exitvalue);

void thread_dump(thread_t *t);

void thread_panic_dump(void);

/* -------------------------------------------------------------------------- */
/*   PROCESSES                                                                */      
/* -------------------------------------------------------------------------- */

void process_setup(void);

process_t *process(const char *name, bool user);

void process_delete(process_t *process);

process_t *process_getbyid(PROCESS process);

int process_count(void);

bool process_cancel(process_t* self, int exitvalue);

void process_exit(int exitvalue);

PROCESS process_exec(const char *executable_path, const char **argv);

void process_filedescriptor_close_all(process_t* this);

int process_filedescriptor_alloc_and_acquire(process_t *this, stream_t *stream);

stream_t *process_filedescriptor_acquire(process_t *this, int fd_index);

int process_filedescriptor_release(process_t *this, int fd_index);

int process_filedescriptor_free_and_release(process_t *this, int fd_index);

int process_open_file(process_t* this, const char *file_path, iostream_flag_t flags);

int process_close_file(process_t* this, int fd);

int process_read_file(process_t* this, int fd, void *buffer, uint size);

int process_write_file(process_t* this, int fd, const void *buffer, uint size);

int process_ioctl_file(process_t* this, int fd, int request, void *args);

int process_seek_file(process_t* this, int fd, int offset, iostream_whence_t whence);

int process_tell_file(process_t* this, int fd, iostream_whence_t whence);

int process_fstat_file(process_t* this, int fd, iostream_stat_t *stat);

int process_memory_map(process_t* p, uint addr, uint count);

int process_memory_unmap(process_t* p, uint addr, uint count);

uint process_memory_alloc(process_t* p, uint count);

void process_memory_free(process_t* p, uint addr, uint count);

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

int messaging_send_internal(PROCESS from, PROCESS to, int id, const char *name, void *payload, uint size, uint flags);

int messaging_send(PROCESS to, const char *name, void *payload, uint size, uint flags);

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

void sheduler_setup(thread_t* main_kernel_thread, process_t* kernel_process);

void wakeup_sleeping_threads(void);

reg32_t shedule(reg32_t sp, processor_context_t *context);

uint sheduler_get_ticks(void);

bool sheduler_is_context_switch(void);

void sheduler_yield(void);

process_t* sheduler_running_process(void);

int sheduler_running_process_id(void);

thread_t* sheduler_running_thread(void);

int sheduler_running_thread_id(void);
