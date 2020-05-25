#pragma once

#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>
#include <libsystem/lock.h>

struct FsNode;
struct FsHandle;

typedef Result (*FsOperationOpen)(struct FsNode *node, struct FsHandle *handle);
typedef void (*FsOperationClose)(struct FsNode *node, struct FsHandle *handle);

typedef bool (*FsOperationCanRead)(struct FsNode *node, struct FsHandle *handle);
typedef bool (*FsOperationCanWrite)(struct FsNode *node, struct FsHandle *handle);

typedef Result (*FsOperationRead)(struct FsNode *node, struct FsHandle *handle, void *buffer, size_t size, size_t *read);
typedef Result (*FsOperationWrite)(struct FsNode *node, struct FsHandle *handle, const void *buffer, size_t size, size_t *written);

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

typedef void (*FsOperationDestroy)(struct FsNode *node);

typedef struct FsNode
{
    FileType type;
    Lock lock;
    uint refcount;

    uint readers;
    uint writers;
    uint clients;
    uint server;
    uint master;

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
    FsOperationAcceptConnection accept_connection;
    FsOperationCanAcceptConnection can_accept_connection;

    FsOperationAccept accept;
    FsOperationIsAccepted is_accepted;

    FsOperationDestroy destroy;
} FsNode;

#define FSNODE(__subclass) ((FsNode *)(__subclass))

void fsnode_init(FsNode *node, FileType type);

FsNode *fsnode_ref(FsNode *node);

void fsnode_deref(FsNode *node);

FsNode *fsnode_ref_handle(FsNode *node, OpenFlag flags);

void fsnode_deref_handle(FsNode *node, OpenFlag flags);

bool fsnode_can_read(FsNode *node, struct FsHandle *handle);

bool fsnode_can_write(FsNode *node, struct FsHandle *handle);

bool fsnode_can_accept(FsNode *node);

bool fsnode_is_accepted(FsNode *node);

bool fsnode_is_acquire(FsNode *node);

void fsnode_acquire_lock(FsNode *node, int who_acquire);

void fsnode_release_lock(FsNode *node, int who_release);
