#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/Result.h>
#include <libsystem/RingBuffer.h>
#include <libsystem/io/Stream.h>
#include <libsystem/lock.h>

#include "abi/Message.h"

struct FsNode;
struct FsHandle;

typedef Result (*FsOperationOpen)(struct FsNode *node, struct FsHandle *handle);
typedef void (*FsOperationClose)(struct FsNode *node, struct FsHandle *handle);

typedef bool (*FsOperationCanRead)(struct FsNode *node, struct FsHandle *handle);
typedef bool (*FsOperationCanWrite)(struct FsNode *node, struct FsHandle *handle);

typedef Result (*FsOperationRead)(struct FsNode *node, struct FsHandle *handle, void *buffer, size_t size, size_t *readed);
typedef Result (*FsOperationWrite)(struct FsNode *node, struct FsHandle *handle, const void *buffer, size_t size, size_t *writen);

typedef struct FsNode *(*FsOperationFind)(struct FsNode *node, const char *name);
typedef Result (*FsOperationLink)(struct FsNode *node, const char *name, struct FsNode *child);
typedef Result (*FsOperationUnlink)(struct FsNode *node, const char *name);

typedef Result (*FsOperationCall)(struct FsNode *node, struct FsHandle *handle, int request, void *args);
typedef Result (*FsOperationStat)(struct FsNode *node, struct FsHandle *handle, FileState *stat);

typedef size_t (*FsOperationSize)(struct FsNode *node, struct FsHandle *handle);

typedef struct FsNode *(*FsOperationOpenConnection)(struct FsNode *node);

typedef void (*FsOperationAccept)(struct FsNode *node);
typedef bool (*FsOperationIsAccepted)(struct FsNode *node);

typedef bool (*FsOperationCanAcceptConnection)(struct FsNode *node);
typedef struct FsNode *(*FsOperationAcceptConnection)(struct FsNode *node);

typedef Result (*FsOperationSend)(struct FsNode *node, struct FsHandle *handle, Message *message);

typedef bool (*FsOperationCanReceive)(struct FsNode *node, struct FsHandle *handle);
typedef Result (*FsOperationReceive)(struct FsNode *node, struct FsHandle *handle, Message **message);

typedef void (*FsOperationDestroy)(struct FsNode *node);

typedef enum
{
    FSNODE_UNKNOWN,

    FSNODE_FILE,
    FSNODE_DEVICE,
    FSNODE_DIRECTORY,
    FSNODE_PIPE,
    FSNODE_SOCKET,
    FSNODE_CONNECTION,
    FSNODE_TERMINAL,
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

    FsOperationOpenConnection open_connection;

    FsOperationCanAcceptConnection can_accept_connection;
    FsOperationAcceptConnection accept_connection;

    FsOperationAccept accept;
    FsOperationIsAccepted is_accepted;

    FsOperationSend send;
    FsOperationCanReceive can_receive;
    FsOperationReceive receive;

    FsOperationDestroy destroy;
} FsNode;

#define FSNODE(__subclass) ((FsNode *)(__subclass))

void fsnode_init(FsNode *node, FsNodeType type);

FsNode *fsnode_ref(FsNode *node);

void fsnode_deref(FsNode *node);

bool fsnode_can_read(FsNode *node, struct FsHandle *handle);

bool fsnode_can_write(FsNode *node, struct FsHandle *handle);

bool fsnode_can_accept(FsNode *node);

bool fsnode_can_receive(FsNode *node, struct FsHandle *handle);

bool fsnode_is_accepted(FsNode *node);

bool fsnode_is_acquire(FsNode *node);

void fsnode_acquire_lock(FsNode *node, int who_acquire);

void fsnode_release_lock(FsNode *node, int who_release);
