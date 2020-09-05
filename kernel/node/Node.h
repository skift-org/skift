#pragma once

#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>
#include <libsystem/thread/Lock.h>
#include <libutils/ResultOr.h>

struct FsNode;
struct FsHandle;

typedef Result (*FsNodeOpenCallback)(struct FsNode *node, struct FsHandle *handle);
typedef void (*FsNodeCloseCallback)(struct FsNode *node, struct FsHandle *handle);

typedef struct FsNode *(*FsNodeFindCallback)(struct FsNode *node, const char *name);
typedef Result (*FsNodeLinkCallback)(struct FsNode *node, const char *name, struct FsNode *child);
typedef Result (*FsNodeUnlinkCallback)(struct FsNode *node, const char *name);

typedef Result (*FsNodeCallCallback)(struct FsNode *node, struct FsHandle *handle, IOCall request, void *args);
typedef Result (*FsNodeStatCallback)(struct FsNode *node, struct FsHandle *handle, FileState *stat);

typedef size_t (*FsNodeSizeCallback)(struct FsNode *node, struct FsHandle *handle);

typedef struct FsNode *(*FsNodeOpenConnectionCallback)(struct FsNode *node);

typedef void (*FsNodeAcceptCallback)(struct FsNode *node);
typedef bool (*FsNodeIsAcceptedCallback)(struct FsNode *node);

typedef bool (*FsNodeCanAcceptConnectionCallback)(struct FsNode *node);
typedef struct FsNode *(*FsNodeAcceptConnectionCallback)(struct FsNode *node);

typedef void (*FsNodeDestroyCallback)(struct FsNode *node);

struct FsNode
{
    FileType type;
    Lock lock;
    uint refcount;

    uint readers;
    uint writers;
    uint clients;
    uint server;
    uint master;

    FsNodeOpenCallback open = nullptr;
    FsNodeCloseCallback close = nullptr;
    FsNodeFindCallback find = nullptr;
    FsNodeLinkCallback link = nullptr;
    FsNodeUnlinkCallback unlink = nullptr;
    FsNodeCallCallback call = nullptr;
    FsNodeStatCallback stat = nullptr;
    FsNodeSizeCallback size = nullptr;

    FsNodeOpenConnectionCallback open_connection;
    FsNodeAcceptConnectionCallback accept_connection;
    FsNodeCanAcceptConnectionCallback can_accept_connection;

    FsNodeAcceptCallback accept;
    FsNodeIsAcceptedCallback is_accepted;

    FsNodeDestroyCallback destroy;

public:
    FsNode(FileType type);

    FsNode *ref();

    void deref();

    void ref_handle(FsHandle &handle);

    void deref_handle(FsHandle &handle);

    virtual bool can_read(FsHandle *handle)
    {
        __unused(handle);

        return true;
    }

    virtual bool can_write(FsHandle *handle)
    {
        __unused(handle);

        return true;
    }

    virtual ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size)
    {
        __unused(handle);
        __unused(buffer);
        __unused(size);

        return ERR_NOT_READABLE;
    }

    virtual ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size)
    {
        __unused(handle);
        __unused(buffer);
        __unused(size);

        return ERR_NOT_WRITABLE;
    }
};

bool fsnode_can_accept(FsNode *node);

bool fsnode_is_accepted(FsNode *node);

bool fsnode_is_acquire(FsNode *node);

void fsnode_acquire_lock(FsNode *node, int who_acquire);

void fsnode_release_lock(FsNode *node, int who_release);
