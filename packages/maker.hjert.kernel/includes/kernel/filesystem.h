#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>
#include <skift/list.h>
#include <skift/path.h>

#include "kernel/shared/filesystem.h"

struct fsnode;

typedef struct
{
    int size;

    bool read;
    bool write;
} fstat_t;

typedef int (*fsop_open_t)(struct fsnode *n);
typedef void (*fsop_close_t)(struct fsnode *n);
typedef int (*fsop_read_t)(struct fsnode *n, uint offset, void *buffer, uint n);
typedef int (*fsop_write_t)(struct fsnode *n, uint offset, void *buffer, uint n);

typedef enum 
{
    FSFILE,
    FSDEVICE,
    FSDIRECTORY,
} fsnode_type_t;

typedef struct
{
    fsop_open_t  open;
    fsop_close_t close;
    fsop_read_t  read;
    fsop_write_t write;
} device_t;

typedef struct directory
{
    list_t *childs;
} directory_t;

typedef struct file
{
    bool opened;
    byte *buffer;
    int size;
} file_t;

typedef struct fsnode
{
    char name[FSNAME_SIZE];
    fsnode_type_t type;

    union
    {
        file_t file;
        directory_t directory;
    };

    int refcount;
} fsnode_t;

void filesystem_setup(void);
fsnode_t *filesystem_resolve(fsnode_t *relative, const char *path);

/* --- Files Operation ------------------------------------------------------ */
fsnode_t *filesystem_open(fsnode_t *relative, const char *path);
void filesystem_close(fsnode_t *node);

int filesystem_read(fsnode_t *node, uint offset, void *buffer, uint n);
int filesystem_write(fsnode_t *node, uint offset, void *buffer, uint n);

int filesystem_mkdir(fsnode_t *relative, const char *path);