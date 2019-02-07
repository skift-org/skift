#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#define FSPATH_SEP '/'
#define FSNAME_SIZE 128

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
    char name[FSNAME_SIZE];
    fsnode_type_t type;
} directory_entry_t;