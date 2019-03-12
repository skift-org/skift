#pragma once

#include <skift/generic.h>
#include <skift/list.h>
#include <skift/lock.h>

#include "kernel/filesystem.h"
#include "kernel/paging.h"
#include "kernel/limits.h"

typedef int PROCESS; // Process handler

typedef enum e_process_state
{
    PROCESS_RUNNING,

    // This is like a *zombie* process in **UNIX**.
    // But in **hjert** if your parent die you die with him.
    // This is why we have **srvhost**.
    PROCESS_CANCELING,

    PROCESS_CANCELED, // This process is ready to be garbage colected.
} process_state_t;

typedef struct s_filedescriptor
{
    bool used;
    stream_t stream;
} filedescriptor_t;

typedef struct s_process
{
    int id;                          // Unique handle to the process
    bool user;                       // Is this a user process
    char name[MAX_PROCESS_NAMESIZE]; // Frendly name of the process
    struct s_process *parent;        // Our parent

    filedescriptor_t filedescriptors[MAX_PROCESS_OPENED_FILES];

    list_t *threads;   // Child threads
    list_t *processes; // Child processes

    list_t *inbox;  // process main message queu
    list_t *shared; // Shared memory region

    page_directorie_t *pdir; // Page directorie
    process_state_t state;   // State of the process (RUNNING, CANCELED)

    int exitvalue;
} process_t;

process_t *alloc_process(const char *name, bool user);


