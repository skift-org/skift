#pragma once

#include <libsystem/Lock.h>
#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>

struct FsNode;
struct FsHandle;

typedef Result (*FsNodeOpenCallback)(struct FsNode *node, struct FsHandle *handle);
typedef void (*FsNodeCloseCallback)(struct FsNode *node, struct FsHandle *handle);

typedef bool (*FsNodeCanReadCallback)(struct FsNode *node, struct FsHandle *handle);
typedef bool (*FsNodeCanWriteCallback)(struct FsNode *node, struct FsHandle *handle);

typedef Result (*FsNodeReadCallback)(struct FsNode *node, struct FsHandle *handle, void *buffer, size_t size, size_t *read);
typedef Result (*FsNodeWriteCallback)(struct FsNode *node, struct FsHandle *handle, const void *buffer, size_t size, size_t *written);

typedef struct FsNode *(*FsNodeFindCallback)(struct FsNode *node, const char *name);
typedef Result (*FsNodeLinkCallback)(struct FsNode *node, const char *name, struct FsNode *child);
typedef Result (*FsNodeUnlinkCallback)(struct FsNode *node, const char *name);

typedef Result (*FsNodeCallCallback)(struct FsNode *node, struct FsHandle *handle, int request, void *args);
typedef Result (*FsNodeStatCallback)(struct FsNode *node, struct FsHandle *handle, FileState *stat);

typedef size_t (*FsNodeSizeCallback)(struct FsNode *node, struct FsHandle *handle);

typedef struct FsNode *(*FsNodeOpenConnectionCallback)(struct FsNode *node);

typedef void (*FsNodeAcceptCallback)(struct FsNode *node);
typedef bool (*FsNodeIsAcceptedCallback)(struct FsNode *node);

typedef bool (*FsNodeCanAcceptConnectionCallback)(struct FsNode *node);
typedef struct FsNode *(*FsNodeAcceptConnectionCallback)(struct FsNode *node);

typedef void (*FsNodeDestroyCallback)(struct FsNode *node);

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

    FsNodeOpenCallback open;
    FsNodeCloseCallback close;
    FsNodeCanReadCallback can_read;
    FsNodeCanWriteCallback can_write;
    FsNodeReadCallback read;
    FsNodeWriteCallback write;
    FsNodeFindCallback find;
    FsNodeLinkCallback link;
    FsNodeUnlinkCallback unlink;
    FsNodeCallCallback call;
    FsNodeStatCallback stat;
    FsNodeSizeCallback size;

    FsNodeOpenConnectionCallback open_connection;
    FsNodeAcceptConnectionCallback accept_connection;
    FsNodeCanAcceptConnectionCallback can_accept_connection;

    FsNodeAcceptCallback accept;
    FsNodeIsAcceptedCallback is_accepted;

    FsNodeDestroyCallback destroy;
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
