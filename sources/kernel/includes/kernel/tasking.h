#pragma once

/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */
#include <types.h>
#include "utils.h"

#include "ds/list.h"

#include "kernel/paging.h"

#define PROCNAME_SIZE 128
#define STACK_SIZE 0x4000

#define TASK_USER 1

typedef int THREAD;  // Thread handle
typedef int PROCESS; // Process handler

typedef u32 esp_t;
typedef void *(*thread_entry_t)(void *);

typedef enum process_state
{
    PROCESS_RUNNING,
    PROCESS_CANCELING,
    PROCESS_CANCELED,
} process_state_t;

typedef enum thread_state
{
    THREAD_RUNNING,
    THREAD_SLEEP,

    THREAD_WAIT_THREAD,
    THREAD_WAIT_PROCESS,

    THREAD_CANCELING,
    THREAD_CANCELED,
} thread_state_t;

typedef struct
{
    int id;                   // Unique handle to the process
    char name[PROCNAME_SIZE]; // Frendly name of the process

    int flags;
    list_t *threads; // Child threads

    page_directorie_t *pdir; // Page directorie
    process_state_t state;   // State of the process (RUNNING, CANCELED)

    int exit_code;
} process_t;

typedef struct
{
    int handle;
    int outcode;
} wait_info_t;

typedef struct
{
    uint wakeuptick;
} sleep_info_t;

typedef struct
{
    int id;
    process_t *process;
    thread_entry_t entry;

    uint esp;
    void *stack;

    thread_state_t state;

    wait_info_t waitinfo;
    sleep_info_t sleepinfo;

    void *exit_value;
} thread_t;

void tasking_setup();

/* --- Thread managment ----------------------------------------------------- */

THREAD thread_self(); // Return a handle to the current thread.

// Create a new thread of a selected process.
THREAD thread_create(PROCESS p, thread_entry_t entry, void *arg, int flags);

int thread_cancel(THREAD t);    // Cancel the selected thread.
void thread_exit(void *retval); // Exit the current thread and return a value.

void thread_sleep(int time);  // Send the current thread to bed.
void thread_wakeup(THREAD t); // Wake up the slected thread

void *thread_wait(THREAD t);    // Wait for the selected thread to exit and return the exit value
int thread_waitproc(PROCESS p); // Wait for the slected process to exit and return the exit code.

void thread_yield(); // Yield to the next thread.

void thread_dump_all();
void thread_dump(THREAD t);

/* --- Process managment ---------------------------------------------------- */

PROCESS process_self(); // Return a handler to the current process.

// Create a new process.
PROCESS process_create(const char *name, int flags);

void process_cancel(PROCESS p); // Cancle the selected process.
void process_exit(int code);    // Exit the current process and send and exit code.

int process_map(PROCESS p, uint addr, uint count);   // Map memory to the process memory space.
int process_unmap(PROCESS p, uint addr, uint count); // Unmap memory from the current thread.

uint process_alloc(uint count);           // Alloc some some memory page to the process memory space.
void process_free(uint addr, uint count); // Free perviously allocated memory.

// Load a ELF executable, create a adress space and run it.
PROCESS process_exec(const char *filename, const char **argv);