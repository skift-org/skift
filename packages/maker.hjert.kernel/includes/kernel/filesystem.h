#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>
#include <skift/list.h>
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

    byte *buffer;
    int size;
    int realsize;
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

/* --- Files Operation ------------------------------------------------------ */
fsnode_t *filesystem_open(fsnode_t *relative, const char *path, fsopenopt_t option);
void filesystem_close(fsnode_t *node);

int  filesystem_read(fsnode_t *node, uint offset, void *buffer, uint n);
int filesystem_write(fsnode_t *node, uint offset, void *buffer, uint n);

int filesystem_mkdir(fsnode_t *relative, const char *path);