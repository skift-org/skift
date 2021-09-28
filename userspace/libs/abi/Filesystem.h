#pragma once

#include <libutils/Prelude.h>

#define FILE_NAME_LENGTH (64)
#define PATH_LENGTH (512)
#define PATH_DEPTH (16)
#define PATH_SEPARATOR '/'

enum HjWhence
{
    HJ_WHENCE_START,
    HJ_WHENCE_CURRENT,
    HJ_WHENCE_END,
};

enum HjFileType
{
    HJ_FILE_TYPE_UNKNOWN,

    HJ_FILE_TYPE_REGULAR,
    HJ_FILE_TYPE_DEVICE,
    HJ_FILE_TYPE_DIRECTORY,
    HJ_FILE_TYPE_PIPE,
    HJ_FILE_TYPE_SOCKET,
    HJ_FILE_TYPE_CONNECTION,
    HJ_FILE_TYPE_TERMINAL,
};

#define HJ_OPEN_READ (1 << 0)
#define HJ_OPEN_WRITE (1 << 1)
#define HJ_OPEN_CREATE (1 << 2)
#define HJ_OPEN_APPEND (1 << 3)
#define HJ_OPEN_TRUNC (1 << 4)
#define HJ_OPEN_BUFFERED (1 << 5)
#define HJ_OPEN_STREAM (1 << 6)
#define HJ_OPEN_DIRECTORY (1 << 7)
#define HJ_OPEN_SOCKET (1 << 8)
#define HJ_OPEN_CLIENT (1 << 9)
#define HJ_OPEN_SERVER (1 << 10)

typedef unsigned int HjOpenFlag;

struct HjStat
{
    size_t size;
    HjFileType type;
};

struct HjDirEntry
{
    char name[FILE_NAME_LENGTH];
    HjStat stat;
};
