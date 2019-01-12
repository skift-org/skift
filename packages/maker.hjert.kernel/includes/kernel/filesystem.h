#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>
#include <skift/list.h>
#include <skift/lock.h>
#include <skift/path.h>

#include "kernel/shared/filesystem.h"

struct fsnode;

typedef int   (*fsop_open_t)(struct fsnode *node);
typedef void (*fsop_close_t)(struct fsnode *node);
typedef int   (*fsop_read_t)(struct fsnode *node, uint offset, void *buffer, uint n);
typedef int  (*fsop_write_t)(struct fsnode *node, uint offset, void *buffer, uint n);

typedef struct
{
    fsop_open_t  open;
    fsop_close_t close;
    fsop_read_t  read;
    fsop_write_t write;
} device_t;

typedef struct
{
    list_t *childs;
} directory_t;

typedef struct
{
    bool opened;

    bool read;
    bool write;

    byte *buffer;
    uint size;
    uint realsize;
} file_t;

typedef struct fsnode
{
    char name[FSNAME_SIZE];
    fsnode_type_t type;
    lock_t lock;

    union
    {
        file_t file;
        directory_t directory;
        device_t device;
    };

    int refcount;
} fsnode_t;

void filesystem_setup(void);

/* --- Files Operation ------------------------------------------------------ */
fsnode_t *filesystem_open(const char *path, fsopenopt_t option);
void filesystem_close(fsnode_t *node);

int  filesystem_read(fsnode_t *node, uint offset, void *buffer, uint n);
void* filesystem_readall(fsnode_t *node);
int filesystem_write(fsnode_t *node, uint offset, void *buffer, uint n);
int filesystem_stat(fsnode_t *node, file_stat_t *stat);

int filesystem_mkdir(const char *path);