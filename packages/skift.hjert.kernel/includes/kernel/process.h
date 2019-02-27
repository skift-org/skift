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

process_t *process(const char *name, bool user);
void process_delete(process_t *process);

process_t *process_byid(int id);

uint process_load_executable(process_t *process, const char *executable);
void process_create_main_thread(process_t* process, void* entry, const char **argv);
void process_create_thread(process_t* process, void* entry, void* arg, bool user);
void process_setready(process_t* process);

uint process_memory_map(process_t* process, uint addr, uint pagecount);
uint process_memory_alloc(process_t* process, uint pagecount);
void process_memory_free(process_t* process, uint addr, uint pagecount);
void process_memory_copyout(process_t* process, void* src, void* dest, uint size);

int process_fildes_alloc(process_t* process);
void process_fildes_set_stream(process_t* process, int fd, stream_t* stream);
void process_fildes_free(process_t* process, int fd);

stream_t* process_fildes_aquired(process_t* process, int fd);
void process_fildes_release(process_t* process, int fd);


