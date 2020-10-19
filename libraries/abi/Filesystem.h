#pragma once

#include <libsystem/Common.h>

#define FILE_NAME_LENGTH (64)
#define PATH_LENGTH (512)
#define PATH_DEPTH (16)
#define PATH_SEPARATOR '/'

enum Whence
{
    WHENCE_START,
    WHENCE_HERE,
    WHENCE_END,
};

enum FileType
{
    FILE_TYPE_UNKNOWN,

    FILE_TYPE_REGULAR,
    FILE_TYPE_DEVICE,
    FILE_TYPE_DIRECTORY,
    FILE_TYPE_PIPE,
    FILE_TYPE_SOCKET,
    FILE_TYPE_CONNECTION,
    FILE_TYPE_TERMINAL,
};

struct Mode
{
public:
    bool u_r;
    bool u_w;
    bool u_x;

    bool g_r;
    bool g_w;
    bool g_x;

    bool o_r;
    bool o_w;
    bool o_x;
};

#define OPEN_READ (1 << 0)
#define OPEN_WRITE (1 << 1)
#define OPEN_CREATE (1 << 2)
#define OPEN_EXECUTE (1 << 3)
#define OPEN_APPEND (1 << 4)
#define OPEN_TRUNC (1 << 5)
#define OPEN_BUFFERED (1 << 6)
#define OPEN_STREAM (1 << 7)
#define OPEN_DIRECTORY (1 << 8)
#define OPEN_SOCKET (1 << 9)
#define OPEN_CLIENT (1 << 10)
#define OPEN_SERVER (1 << 11)
#define OPEN_MASTER (1 << 12)

#define MODE_USER_READ (1 << 0)
#define MODE_USER_WRITE (1 << 1)
#define MODE_USER_EXECUTE (1 << 2)
#define MODE_GROUP_READ (1 << 3)
#define MODE_GROUP_WRITE (1 << 4)
#define MODE_GROUP_EXECUTE (1 << 5)
#define MODE_OTHER_READ (1 << 6)
#define MODE_OTHER_WRITE (1 << 7)
#define MODE_OTHER_EXECUTE (1 << 8)

typedef unsigned int OpenFlag;

struct FileState
{
    size_t size;
    FileType type;
};

struct DirectoryEntry
{
    char name[FILE_NAME_LENGTH];
    FileState stat;
};
