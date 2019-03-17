#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>

#include "kernel/limits.h"

#define FSPATH_SEP '/'

typedef enum
{
    SEEKFROM_START,
    SEEKFROM_END,
    SEEKFROM_HERE
} seek_origin_t;

typedef enum
{
    OPENOPT_READ = 1 << 0,
    OPENOPT_WRITE = 1 << 1,
    OPENOPT_READWRITE = 1 << 2,
    OPENOPT_CREATE = 1 << 3,
    OPENOPT_APPEND = 1 << 4,
    OPENOPT_TRUNC = 1 << 5,
} fsoflags_t;

typedef enum
{
    FSFILE,
    FSDEVICE,
    FSDIRECTORY,
    FSFIFO,
} fsnode_type_t;

typedef enum
{
    FSRESULT_EOF = -128,
    
    FSRESULT_NOTFOUND,
    FSRESULT_NOTSUPPORTED,
    FSRESULT_NULLNODE,

    FSRESULT_READNOTPERMITTED,
    FSRESULT_WRITENOTPERMITTED,

    FSRESULT_SUCCEED,
    FSRESULT_FAILED,
} fsresult_t;

typedef struct 
{
    uint size;
    fsnode_type_t type;
} file_stat_t;

typedef struct
{
    char name[MAX_FILENAME_LENGHT];
    fsnode_type_t type;
} directory_entry_t;