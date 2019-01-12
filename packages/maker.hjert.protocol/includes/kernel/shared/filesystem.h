#pragma once

#define FSPATH_SEP '/'
#define FSNAME_SIZE 128

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

    OPENOPT_CREATE = 1 << 2,
} fsopenopt_t;

typedef enum
{
    FSFILE,
    FSDEVICE,
    FSDIRECTORY,
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
} fsresult_t;

typedef struct 
{
    uint size;
    fsnode_type_t type;
} file_stat_t;

typedef struct
{
    char name[FSNAME_SIZE];
    file_stat_t state;
} directory_entry_t;