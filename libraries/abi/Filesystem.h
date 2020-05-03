#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

#define FILE_NAME_LENGTH (64)
#define PATH_LENGTH (512)
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

#define OPEN_READ (1 << 0)
#define OPEN_WRITE (1 << 1)
#define OPEN_CREATE (1 << 2)
#define OPEN_APPEND (1 << 3)
#define OPEN_TRUNC (1 << 4)
#define OPEN_BUFFERED (1 << 5)
#define OPEN_STREAM (1 << 6)
#define OPEN_DIRECTORY (1 << 7)
#define OPEN_SOCKET (1 << 8)
#define OPEN_CLIENT (1 << 9)
#define OPEN_SERVER (1 << 10)
#define OPEN_MASTER (1 << 11)

typedef unsigned int OpenFlag;

typedef struct
{
    size_t size;
    FileType type;
} FileState;

typedef struct
{
    char name[FILE_NAME_LENGTH];
    FileState stat;
} DirectoryEntry;
