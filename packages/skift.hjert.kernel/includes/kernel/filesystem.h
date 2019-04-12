#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>
#include <skift/path.h>
#include <skift/list.h>
#include <skift/lock.h>
#include <skift/ringbuffer.h>
#include <skift/iostream.h>

#include "kernel/system.h"

#define ROOT NULL

struct s_fsnode;
struct s_stream;

typedef int (*fsop_read_t)(struct s_stream *s, void *buffer, uint size);
typedef int (*fsop_write_t)(struct s_stream *s, void *buffer, uint size);
typedef int (*fsop_ioctl_t)(struct s_stream *s, int request, void *args);

typedef struct
{
    byte *buffer;
    uint realsize;
    uint size;
} file_t;

typedef struct
{
    fsop_read_t read;
    fsop_write_t write;
    fsop_ioctl_t ioctl;

    void *p;
} device_t;

typedef struct
{
    char name[PATH_ELEMENT_LENGHT];
    struct s_fsnode *node;
} fsdirectory_entry_t;

typedef struct
{
    list_t *childs;
} directory_t;

typedef struct
{
    ringbuffer_t *buffer;
} fifo_t;

typedef struct s_fsnode
{
    char name[PATH_ELEMENT_LENGHT];
    iostream_type_t type;
    lock_t lock;

    union {
        file_t file;
        directory_t directory;
        device_t device;
        fifo_t fifo;
    };

    int refcount;
} fsnode_t;

typedef struct
{
    int count;
    iostream_direntry_t *entries;
} directory_entries_t;

typedef struct s_stream
{
    fsnode_t *node;
    uint offset;
    iostream_flag_t flags;

    union {
        directory_entries_t direntries;
    };
} stream_t;

void filesystem_setup(void);
void filesystem_panic_dump(void);

/* --- fsnode --------------------------------------------------------------- */

// Resolve and acquire a fsnode.
fsnode_t *filesystem_acquire(fsnode_t *at, path_t* p, bool create);

// Release the fsnode.
void filesystem_release(fsnode_t *node);

/* --- File IO -------------------------------------------------------------- */
stream_t *filesystem_open(fsnode_t *at, path_t* p, iostream_flag_t flags);
void filesystem_close(stream_t *s);

int filesystem_read(stream_t *s, void *buffer, uint size);
int filesystem_write(stream_t *s, void *buffer, uint size);

int filesystem_ioctl(stream_t *s, int request, void *args);

int filesystem_seek(stream_t *s, int offset, iostream_whence_t origine);
int filesystem_tell(stream_t *s, iostream_whence_t whence);

int filesystem_fstat(stream_t *s, iostream_stat_t *stat);

void *filesystem_readall(stream_t *s);

/* --- File system operation ------------------------------------------------ */
int filesystem_mkfile(fsnode_t *at, path_t* p);
int filesystem_mkfifo(fsnode_t *at, path_t* p); /* TODO */
int filesystem_mkdev(fsnode_t *at, path_t* p, device_t dev);
int filesystem_mkdir(fsnode_t *at, path_t* p);

int filesystem_link(fsnode_t * atfile, path_t* file, fsnode_t *atlink, path_t* link);
int filesystem_unlink(fsnode_t *at, path_t* p);

// *filesystem_mkdev* with error checking.
#define FILESYSTEM_MKDEV(__name, __object)                           \
    path_t* __dev_path = path("/dev/" __name );                      \
    sk_log(LOG_DEBUG, "Creating device " __name " at /dev/" __name); \
    if (filesystem_mkdev(NULL, __dev_path, (__object)))              \
    {                                                                \
        PANIC("Failled to create the '" __name "' device.");         \
    }                                                                \
    path_delete(__dev_path);
