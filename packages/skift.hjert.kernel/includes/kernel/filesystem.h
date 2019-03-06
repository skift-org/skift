#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>
#include <skift/list.h>
#include <skift/lock.h>
#include <skift/path.h>
#include <skift/ringbuffer.h>

#include "kernel/shared/filesystem.h"

struct fsnode;
struct stream;

typedef int (*fsop_read_t)(struct stream *s, uint size, void *buffer);
typedef int (*fsop_write_t)(struct stream *s, uint size, void *buffer);

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

    void *p;
} device_t;

typedef struct
{
    list_t *childs;
} directory_t;

typedef struct
{
    ringbuffer_t *buffer;
} fifo_t;

typedef struct fsnode
{
    char name[MAX_FILENAME_LENGHT];
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
    directory_entry_t *entries;
} directory_entries_t;

typedef struct stream
{
    fsnode_t *node;
    uint offset;
    fsoflags_t flags;

    union {
        directory_entries_t direntries;
    };
} stream_t;

void filesystem_setup(void);
void filesystem_dump(void);

/* --- File IO -------------------------------------------------------------- */
stream_t *filesystem_open(const char *path, fsoflags_t flags);
void filesystem_close(stream_t *s);

int filesystem_read(stream_t *s, void *buffer, uint size);
int filesystem_write(stream_t *s, void *buffer, uint size);

int filesystem_seek(stream_t *s, int offset, seek_origin_t origine);
int filesystem_tell(stream_t *s);

int filesystem_fstat(stream_t *s, file_stat_t *stat);

void *filesystem_readall(stream_t *s);

/* --- File system operation ------------------------------------------------ */
int filesystem_mkfile(const char *path);
int filesystem_mkfifo(const char *path); /* TODO */
int filesystem_mkdev(const char *path, device_t dev);
int filesystem_mkdir(const char *path);

int filesystem_rm(const char *path);
