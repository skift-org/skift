#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

#define FILE_NAME_LENGHT (64)
#define PATH_LENGHT (512)
#define PATH_DEPTH (16)
#define PATH_SEPARATOR '/'

typedef enum
{
    WHENCE_START,
    WHENCE_HERE,
    WHENCE_END,
} Whence;

typedef enum
{
    FILE_TYPE_UNKNOWN,

    FILE_TYPE_REGULAR,
    FILE_TYPE_DEVICE,
    FILE_TYPE_DIRECTORY,
    FILE_TYPE_PIPE,
    FILE_TYPE_SOCKET,
    FILE_TYPE_CONNECTION,
    FILE_TYPE_TERMINAL,
} FileType;

typedef enum
{
    OPEN_READ = 1 << 0,
    OPEN_WRITE = 1 << 1,
    OPEN_CREATE = 1 << 2,
    OPEN_APPEND = 1 << 3,
    OPEN_TRUNC = 1 << 4,
    OPEN_BUFFERED = 1 << 5,

    OPEN_STREAM = 1 << 6,
    OPEN_DIRECTORY = 1 << 7,
    OPEN_SOCKET = 1 << 8,

    OPEN_CLIENT = 1 << 9,
    OPEN_SERVER = 1 << 10,

    OPEN_MASTER = 1 << 11,
} OpenFlag;

typedef struct
{
    size_t size;
    FileType type;
} FileState;

typedef struct
{
    char name[FILE_NAME_LENGHT];
    FileState stat;
} DirectoryEntry;
