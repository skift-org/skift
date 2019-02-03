#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>
#include <skift/list.h>

#include "kernel/paging.h"
#include "kernel/protocol.h"
#include "kernel/limits.h"

/* --- Process -------------------------------------------------------------- */

typedef int PROCESS; // Process handler

typedef enum process_state
{
    PROCESS_RUNNING,
    PROCESS_CANCELING,
    PROCESS_CANCELED,
} process_state_t;

typedef struct
{
    int id;                          // Unique handle to the process
    char name[MAX_PROCESS_NAMESIZE]; // Frendly name of the process

    bool user;
    list_t *threads; // Child threads;
    list_t *inbox;   // process main message queu
    list_t *shared;  // Shared memory region;

    page_directorie_t *pdir; // Page directorie
    process_state_t state;   // State of the process (RUNNING, CANCELED)

    int exitvalue;
} process_t;

/* --- Thread --------------------------------------------------------------- */

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
    process_t *process;
    thread_entry_t entry;

    thread_state_t state;

    bool user;
    uint sp;
    byte stack[MAX_THREAD_STACKSIZE];

    struct
    {
        thread_wait_time_t time;
        thread_wait_process_t process;
        thread_wait_thread_t thread;
        thread_wait_message_t message;
    } wait;

    int exitvalue;
} thread_t;

/* --- Tasking initialisation ----------------------------------------------- */

void tasking_setup();

/* --- Thread managment ----------------------------------------------------- */

THREAD thread_self(); // Return a handle to the current thread.
thread_t *thread_running();
thread_t *thread_getbyid(int id);
void thread_hold();
void thread_setstate(thread_t *thread, thread_state_t state);

THREAD thread_create(PROCESS p, thread_entry_t entry, void *arg, bool user); // Create a new thread of a selected process.

bool thread_cancel(THREAD t, int exitvalue); // Cancel the selected thread.
void thread_exit(int exitvalue);             // Exit the current thread and return a value.

void thread_sleep(int time);  // Send the current thread to bed.
void thread_wakeup(THREAD t); // Wake up the slected thread

bool thread_wait_thread(THREAD t, int *exitvalue);   // Wait for the selected thread to exit and return the exit value
bool thread_wait_process(PROCESS p, int *exitvalue); // Wait for the selected process to exit and return the exit value.
void thread_yield();                                 // Yield to the next thread.

void thread_dump_all();
void thread_dump(thread_t *t);

/* --- Process managment ---------------------------------------------------- */

PROCESS process_self(); // Return a handler to the current process.
process_t *process_running();
process_t *process_get(PROCESS process);

PROCESS process_create(const char *name, bool user); // Create a new process.

bool process_cancel(PROCESS p, int exitvalue); // Cancel the selected process.
void process_exit(int code);                   // Exit the current process and send a exit code.

int process_map(PROCESS p, uint addr, uint count);   // Map memory to the process memory space.
int process_unmap(PROCESS p, uint addr, uint count); // Unmap memory from the current thread.

uint process_alloc(uint count);           // Alloc some some memory page to the process memory space.
void process_free(uint addr, uint count); // Free perviously allocated memory.

PROCESS process_exec(const char *filename, const char **argv); // Load a ELF executable, create a adress space and run it.
