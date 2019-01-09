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
    OPENOPT_APPEND = 1 << 3,
} open_option_t;

typedef enum
{
    FSNODE_FILE,
    FSNODE_DEVICE,
    FSNODE_LINK,
    FSNODE_DIRECTORY
} fsnode_type_t;

static const char* fsnode_type_str[] = 
{
    "FILE",
    "DEVICE",
    "LINK",
    "DIRECTORY"
};

typedef struct 
{
    uint size;
    fsnode_type_t type;
} file_state_t;

typedef struct
{
    char name[FSNAME_SIZE];
    file_state_t state;
} directory_entry_t;