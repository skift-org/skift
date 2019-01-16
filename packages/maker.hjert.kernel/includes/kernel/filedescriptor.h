#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/lock.h>

#include "kernel/limits.h"
#include "kernel/filesystem.h"

typedef struct 
{
    bool isFree;
    lock_t lock;
    stream_t* stream;
} filedescriptor_t;

typedef struct 
{
    lock_t lock;
    filedescriptor_t fds[MAX_PROCESS_OPENED_FILES];
} fdtable_t;


fdtable_t* fdtable();
void fdtable_delete(fdtable_t* fdt);

int fdalloc(fdtable_t* table);
void fdfree(fdtable_t* table, int fd);