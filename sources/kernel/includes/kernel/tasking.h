#pragma once

#include <stdbool.h>

#include "types.h"
#include "utils.h"

#include "ds/list.h"

#include "kernel/paging.h"

#define PROCNAME_SIZE 128
#define STACK_SIZE 4096

#define TASK_USER 1

typedef int THREAD;
typedef int PROCESS;

typedef u32 esp_t;
typedef void *(*thread_entry_t)(void *);

typedef enum
{
    PROCESS_RUNNING,
    PROCESS_CANCELED,
} process_state_t;

typedef enum
{
    THREAD_RUNNING,
    THREAD_SLEEP,

    THREAD_WAIT_THREAD,
    THREAD_WAIT_PROCESS,
    
    THREAD_CANCELED,
} thread_state_t;

typedef struct
{
    int id;
    char name[PROCNAME_SIZE];
    int flags;
    list_t *threads;
    page_directorie_t *pdir;

    process_state_t state;
} process_t;

typedef struct 
{
    int handle;
    int * outcode;
} wait_info_t;

typedef struct
{
    int id;
    process_t *process;
    thread_entry_t entry;

    uint esp;
    void *stack;

    thread_state_t state;
    wait_info_t waitinfo;
} thread_t;

void tasking_setup();

/* --- Thread managment ----------------------------------------------------- */

THREAD thread_self();

THREAD thread_create(PROCESS p, thread_entry_t entry, void *arg, int flags);

int thread_cancel(THREAD t);
void thread_exit(void *retval);

void thread_sleep();
void thread_wakeup(THREAD t);

void *thread_wait(THREAD t);
int thread_waitproc(PROCESS p);


/* --- Process managment ---------------------------------------------------- */

PROCESS process_self();

PROCESS process_create(const char *name, int flags);
void process_cancel(PROCESS p);
void process_exit(int code);

int process_map(PROCESS p, uint addr, uint count);
int process_unmap(PROCESS p, uint addr, uint count);

PROCESS process_exec(const char *filename, int argc, char **argv);