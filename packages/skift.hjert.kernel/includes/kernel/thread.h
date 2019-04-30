#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>
#include <skift/list.h>

#include "kernel/process.h"
#include "kernel/protocol.h"
#include "kernel/limits.h"

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

void thread_setup(void);

thread_t *thread();
void thread_delete(thread_t *thread);

void thread_panic_dump(void);
void thread_dump(thread_t *t);
int thread_count(void);

thread_t *thread_getbyid(int id);
void thread_setstate(thread_t *thread, thread_state_t state);
list_t* thread_bystate(thread_state_t state);
void thread_setentry(thread_t *t, thread_entry_t entry, bool user);
uint thread_stack_push(thread_t *t, const void *value, uint size);
void thread_attach_to_process(thread_t *t, process_t *p);
void thread_setready(thread_t *t);