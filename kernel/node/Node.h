#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/iostream.h>
#include <libsystem/lock.h>
#include <libsystem/RingBuffer.h>

struct FsNode;
struct FsHandle;

typedef int (*FsOperationOpen)(struct FsNode *node, struct FsHandle *handle);
typedef void (*FsOperationClose)(struct FsNode *node, struct FsHandle *handle);

typedef bool (*FsOperationCanRead)(struct FsNode *node);
typedef bool (*FsOperationCanWrite)(struct FsNode *node);

typedef int (*FsOperationRead)(struct FsNode *node, struct FsHandle *handle, void *buffer, size_t size);
typedef int (*FsOperationWrite)(struct FsNode *node, struct FsHandle *handle, const void *buffer, size_t size);

typedef struct FsNode *(*FsOperationFind)(struct FsNode *node, const char *name);
typedef int (*FsOperationLink)(struct FsNode *node, const char *name, struct FsNode *child);
typedef int (*FsOperationUnlink)(struct FsNode *node, const char *name);

typedef int (*FsOperationCall)(struct FsNode *node, struct FsHandle *handle, int request, void *args);
typedef int (*FsOperationStat)(struct FsNode *node, struct FsHandle *handle, FileState *stat);

typedef size_t (*FsOperationSize)(struct FsNode *node, struct FsHandle *handle);

typedef void (*FsOperationDestroy)(struct FsNode *node);

typedef enum
{
    FSNODE_UNKNOWN,

    FSNODE_FILE,
    FSNODE_DEVICE,
    FSNODE_DIRECTORY,
    FSNODE_PIPE,
} FsNodeType;

typedef struct FsNode
{
    FsNodeType type;
    Lock lock;

    // FIXME: is this the right way to do that ?
    _Atomic int refcount;

    FsOperationOpen open;
    FsOperationClose close;
    FsOperationCanRead can_read;
    FsOperationCanRead can_write;
    FsOperationRead read;
    FsOperationWrite write;
    FsOperationFind find;
    FsOperationLink link;
    FsOperationUnlink unlink;
    FsOperationCall call;
    FsOperationStat stat;
    FsOperationSize size;
    FsOperationDestroy destroy;
} FsNode;

#define FSNODE(__subclass) ((FsNode *)(__subclass))

void fsnode_init(FsNode *node, FsNodeType type);

FsNode *fsnode_ref(FsNode *this);

void fsnode_deref(FsNode *this);

bool fsnode_can_read(FsNode *node);

bool fsnode_can_write(FsNode *node);

bool fsnode_is_acquire(FsNode *node);

void fsnode_acquire_lock(FsNode *node, int who_acquire);

void fsnode_release_lock(FsNode *node, int who_release);
