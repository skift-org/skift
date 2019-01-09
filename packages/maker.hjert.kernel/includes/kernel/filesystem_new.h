#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

// WIP new file system api

#include <skift/generic.h>
#include <skift/list.h>
#include <skift/path.h>

#include "kernel/shared/filesystem.h"

struct fsnode;

typedef bool  (*fsop_open_t)(struct fsnode* node, open_option_t option);
typedef bool (*fsop_close_t)(struct fsnode* node);

typedef int   (*fsop_read_t)(struct fsnode* node, void* buffer, uint size);
typedef int  (*fsop_write_t)(struct fsnode* node, void* buffer, uint size);

typedef int   (*fsop_tell_t)(struct fsnode* node);
typedef void  (*fsop_seek_t)(struct fsnode* node, int offset, seek_origin_t origin);
typedef void (*fsop_ioctl_t)(struct fsnode* node, int request, void* args);

typedef struct file
{
    void* buffer;
    uint size;
    uint head;
} file_t;

typedef struct device
{
    fsop_open_t open;
    fsop_close_t close;
    fsop_read_t read;
    fsop_write_t write;
    fsop_tell_t tell;
    fsop_seek_t seek;
    fsop_ioctl_t ioctl;

    void* p;
} device_t;

typedef struct link
{
    struct fsnode* to;
} link_t;

typedef struct directory
{
    list_t* childs;
    struct directory *parent;
} directory_t;

typedef struct fsnode
{
    char name[FSNAME_SIZE];
    fsnode_type_t type;

    union
    {
        file_t file;
        device_t device;
        link_t link;
        directory_t directory;
    };

    struct fsnode *parent;

    uint refcount;
} fsnode_t;

void filesystem_setup(void);

fsnode_t* filesystem_open(fsnode_t* relative, const char *path, open_option_t option);
void filesystem_close(fsnode_t* node);
int filesystem_read(fsnode_t* node, void* buffer, uint size);
int filesystem_write(fsnode_t* node, void* buffer, uint size);
void filesystem_seek(fsnode_t* node, int offset, seek_origin_t origin);
int filesystem_tell(fsnode_t* node);
void filesystem_ioctl(fsnode_t* node, int request, void* args);

void filesystem_mkdir(fsnode_t* relative, const char *path);
bool filesystem_rm(fsnode_t* relative, const char *path);