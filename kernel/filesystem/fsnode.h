#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/ringbuffer.h>
#include <libsystem/iostream.h>
#include <libsystem/lock.h>

#include "filesystem/stream.h"

struct fsnode;
struct stream;

typedef int (*fsop_open_t)(struct stream *s);
typedef int (*fsop_close_t)(struct stream *s);
typedef int (*fsop_read_t)(struct stream *s, void *buffer, uint size);
typedef int (*fsop_write_t)(struct stream *s, const void *buffer, uint size);
typedef int (*fsop_call_t)(struct stream *s, int request, void *args);
typedef int (*fsop_stat_t)(struct stream *s, IOStreamState *stat);

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
    fsop_call_t call;

    void *p;
} device_t;

typedef struct
{
    char name[PATH_ELEMENT_LENGHT];
    struct fsnode *node;
} fsdirectory_entry_t;

typedef struct
{
    List *childs;
} directory_t;

typedef struct
{
    ringbuffer_t *buffer;
} fifo_t;

typedef enum
{
    FSNODE_FILE,
    FSNODE_DEVICE,
    FSNODE_FIFO,
    FSNODE_DIRECTORY
} fsnode_type_t;

typedef struct fsnode
{
    char name[PATH_ELEMENT_LENGHT];
    fsnode_type_t type;
    Lock lock;

    union {
        file_t file;
        directory_t directory;
        device_t device;
        fifo_t fifo;
    };

    int refcount;
} fsnode_t;

/* --- fsnode --------------------------------------------------------------- */

fsnode_t *fsnode(fsnode_type_t type);

void fsnode_delete(fsnode_t *node);

int fsnode_size(fsnode_t *node);

IOStreamType fsnode_type(fsnode_t *this);

int fsnode_stat(fsnode_t *node, IOStreamState *stat);

/* --- Files ---------------------------------------------------------------- */

void file_trunc(fsnode_t *node);

int file_read(struct stream *stream, void *buffer, uint size);

int file_write(struct stream *stream, const void *buffer, uint size);

/* --- Fifo ----------------------------------------------------------------- */

int fifo_read(struct stream *stream, void *buffer, uint size);

int fifo_write(struct stream *stream, const void *buffer, uint size);

/* --- Directories ---------------------------------------------------------- */

fsdirectory_entry_t *directory_entry(fsnode_t *dir, const char *child);

bool directory_has_entry(fsnode_t *dir, const char *child);

directory_entries_t directory_entries(fsnode_t *dir);

bool directory_link(fsnode_t *dir, fsnode_t *child, const char *name);

bool directory_unlink(fsnode_t *dir, const char *name);

int directory_read(struct stream *stream, void *buffer, uint size);