#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>
#include <libsystem/path.h>
#include <libsystem/list.h>
#include <libsystem/lock.h>
#include <libsystem/ringbuffer.h>
#include <libsystem/iostream.h>

#include "system.h"

#define ROOT NULL

// struct fsnode;
// struct stream;
//
// // Resole a path to a fsnode, this is used by synthetic file system.
// typedef struct s_fsnode* (*fsop_resolve_t)(void* this, path_t *path);
//
// // Call when a new stream is open
// typedef void (*fsop_open_t)(void* this, struct s_stream *s);
//
// // Call when the stream is close
// typedef void (*fsop_close_t)(void* this, struct s_stream *s);
//
// // Call when we read from the fsnode
// typedef int (*fsop_read_t)(void* this, struct s_stream *s, void *buffer, uint size);
//
// // Call when we write to the fsnode
// typedef int (*fsop_write_t)(void* this, struct s_stream *s, const void *buffer, uint size);
//
// // Call when an ioctl request is made
// typedef int (*fsop_ioctl_t)(void* this, struct s_stream *s, int request, void *args);
//
// // Call when a fstat is called
// typedef int (*fsop_fstat_t)(void* this, struct s_stream *s, iostream_stat_t *stat);
//
// // Call when a new fsnode_t is create
// typedef void (*fsop_init_t)(void* this);
//
// // Call when the fsnode_t is free
// typedef void (*fsop_delete_t)(void* this);
//
struct s_fsnode;
struct s_stream;

typedef int (*fsop_open_t)(struct s_stream *s);
typedef int (*fsop_close_t)(struct s_stream *s);
typedef int (*fsop_read_t)(struct s_stream *s, void *buffer, uint size);
typedef int (*fsop_write_t)(struct s_stream *s, const void *buffer, uint size);
typedef int (*fsop_ioctl_t)(struct s_stream *s, int request, void *args);
typedef int (*fsop_fstat_t)(struct s_stream *s, iostream_stat_t *stat);

typedef struct
{
    byte *buffer;
    uint realsize;
    uint size;
} file_t;

typedef struct
{
    fsop_open_t open;
    fsop_close_t close;

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
    lock_t buffer_lock;
    ringbuffer_t *buffer;
} fifo_t;

typedef enum
{
    FSNODE_FILE,
    FSNODE_DEVICE,
    FSNODE_FIFO,

    FSNODE_DIRECTORY
} fsnode_type_t;

typedef struct s_fsnode
{
    char name[PATH_ELEMENT_LENGHT];
    fsnode_type_t type;
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

typedef struct
{
    void *p;
    int count;

    int current;
} stream_attached_info_t;

typedef struct s_stream
{
    fsnode_t *node;
    uint offset;
    iostream_flag_t flags;

    union {
        directory_entries_t direntries;
        stream_attached_info_t attached;
    };
} stream_t;

iostream_type_t fsnode_to_iostream_type(fsnode_type_t type);

void filesystem_setup(void);
void filesystem_panic_dump(void);

/* --- fsnode --------------------------------------------------------------- */

// Resolve and acquire a fsnode.
fsnode_t *filesystem_acquire(fsnode_t *at, path_t *p, bool create);

// Release the fsnode.
void filesystem_release(fsnode_t *node);

/* --- File IO -------------------------------------------------------------- */

stream_t *filesystem_open(fsnode_t *at, path_t *p, iostream_flag_t flags);

void filesystem_close(stream_t *s);

stream_t *filesystem_dup(stream_t *s);

int filesystem_read(stream_t *s, void *buffer, uint size);

int filesystem_write(stream_t *s, const void *buffer, uint size);

int filesystem_ioctl(stream_t *s, int request, void *args);

int filesystem_seek(stream_t *s, int offset, iostream_whence_t origine);

int filesystem_tell(stream_t *s, iostream_whence_t whence);

int filesystem_fstat(stream_t *s, iostream_stat_t *stat);

/* --- File system operation ------------------------------------------------ */

int filesystem_mkfile(fsnode_t *at, path_t *p);

int filesystem_mkfifo(fsnode_t *at, path_t *p); /* TODO */

int filesystem_mkdev(fsnode_t *at, path_t *p, device_t dev);

int filesystem_mkdir(fsnode_t *at, path_t *p);

int filesystem_link(fsnode_t *atfile, path_t *file, fsnode_t *atlink, path_t *link);

int filesystem_unlink(fsnode_t *at, path_t *p);

int filesystem_rename(fsnode_t *atoldpath, path_t *oldpath, fsnode_t *atnewpath, path_t *newpath);

bool filesystem_exist(fsnode_t *at, path_t *p);

// *filesystem_mkdev* with error checking.
#define FILESYSTEM_MKDEV(__name, __object)                       \
    {                                                            \
        path_t *__dev_path = path(__name);                       \
        logger_info("Creating device " __name " at " __name);    \
        if (filesystem_mkdev(NULL, __dev_path, (__object)))      \
        {                                                        \
            PANIC("Failled to create the '" __name "' device."); \
        }                                                        \
        path_delete(__dev_path);                                 \
    }
