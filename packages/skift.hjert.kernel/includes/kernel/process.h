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
    // But in **HJERT** if your parent die you die with him.
    PROCESS_CANCELING,

    PROCESS_CANCELED, // This process is ready to be garbage colected.
} process_state_t;

typedef struct 
{
    lock_t lock;
    stream_t* stream;
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
    list_t *shared; // Shared memory region

    lock_t fds_lock;
    process_filedescriptor_t fds[MAX_PROCESS_OPENED_FILES];
    
    page_directorie_t *pdir; // Page directorie
    process_state_t state;   // State of the process (RUNNING, CANCELED)

    int exitvalue;
} process_t;

process_t *alloc_process(const char *name, bool user);

// File descriptor allocation and locking ----------------------------------- //

int process_filedescriptor_alloc_and_acquire(process_t* p, stream_t* stream);

stream_t* process_filedescriptor_acquire(process_t* p, int fd);

int process_filedescriptor_release(process_t* p, int fd);

int process_filedescriptor_free_and_release(process_t* p, int fd);

// Process file operations -------------------------------------------------- //

int process_open_file(process_t* process, const char *file_path, iostream_flag_t flags);

int process_close_file(process_t* process, int fd);

int process_read_file(process_t* process, int fd, void *buffer, uint size);

int process_write_file(process_t* process, int fd, void *buffer, uint size);

int process_ioctl_file(process_t* process, int fd, int request, void *args);

int process_seek_file(process_t* process, int fd, int offset, iostream_whence_t whence);

int process_tell_file(process_t* process, int fd, iostream_whence_t whence);

int process_fstat_file(process_t* process, int fd, iostream_stat_t *stat);

