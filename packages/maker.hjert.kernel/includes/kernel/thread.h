#pragma once

#include <skift/generic.h>

#include "kernel/limits.h"
#include "kernel/processor.h"
#include "kernel/protocol.h"

typedef enum
{
    THREAD_EMBRYO,
    THREAD_ALIVE,
    THREAD_WAITING,
    THREAD_DYING,
    THREAD_DEAD,
    THREAD_DUST,
} thread_state_t;

typedef enum
{
    THREAD_PRIORITY_HIGHT,
    THREAD_PRIORITY_LOW,
} thread_priority_t;

typedef struct
{
    uint tick;
} thread_wait_time_t;

typedef struct
{
    int id;
    uint returnvalue;
} thread_wait_thread_t;

typedef struct
{
    int id;
    uint returnvalue;
} thread_wait_process_t;

typedef struct
{
    message_t *message;
} thread_wait_message_t;

typedef struct
{
    int id;
    char name[MAX_THREAD_NAMESIZE];

    bool user;
    thread_state_t state;
    thread_priority_t priority;

    struct
    {
        thread_wait_time_t time;
        thread_wait_thread_t thread;
        thread_wait_process_t process;
        thread_wait_message_t message;
    } wait;

    reg32_t ip;
    reg32_t sp;
    byte stack[MAX_THREAD_STACKSIZE];

    int exitvalue;
} thread_t;

void thread_setup(void);
void thread_dump(void);

thread_t *thread_by_state(int start, thread_state_t state); /* Return the fist thread with a given state */

thread_t *thread(const char *name, bool user); /* Create a new thread object */
void thread_setentrypoint(thread_t *thread, void *entry);
void thread_stackpush(thread_t *thread, void *value, int size);
void thread_ready(thread_t *thread);
void thread_delete(thread_t *thread); /* Delete a thread objet */

void thread_wait_time(uint time);
void thread_wait_thread(int id);
void thread_wait_process(int id);
void thread_wait_message(message_t *message);