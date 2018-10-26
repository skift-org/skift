#pragma once

/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */
#include <skift/types.h>
#include <skift/utils.h>
#include <skift/list.h>

#include "kernel/paging.h"
#include "kernel/shared_protocole.h"

#define CHANNAME_SIZE 128
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
    THREAD_WAIT_MESSAGE,

    THREAD_CANCELING,
    THREAD_CANCELED,
} thread_state_t;

typedef struct
{
    int id;                   // Unique handle to the process
    char name[PROCNAME_SIZE]; // Frendly name of the process

    int flags;
    list_t *threads; // Child threads;
    list_t *inbox;
    list_t *shared; // Shared memory region;

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
    message_t * message;
} wait_message_t;

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
    wait_message_t messageinfo;

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

/* --- Messaging ------------------------------------------------------------ */

typedef struct 
{
    char name[CHANNAME_SIZE];
    list_t * subscribers;
} channel_t;

int messaging_send(PROCESS to, const char * name, void * payload, uint size, uint flags);
int messaging_broadcast(const char * channel, const char * name, void * payload, uint size, uint flags);

status_t messaging_receive(message_t * msg);
status_t messaging_payload(void* buffer, uint size);

status_t messaging_subscribe(const char * channel);
status_t messaging_unsubscribe(const char * channel);

/* --- Shared memory -------------------------------------------------------- */

typedef unsigned int SHARED_MEMORY;

typedef struct 
{
    uint id;
    uint begin;
    uint size;
} shared_memory_t;

SHARED_MEMORY sharedmem_create(uint size);
void sharedmem_destroy(SHARED_MEMORY shm);

uint sharedmem_mount(SHARED_MEMORY shm);
void sharedmem_unmount(SHARED_MEMORY shm);

/* --- Stream --------------------------------------------------------------- */

typedef struct 
{
    
} stream_t;