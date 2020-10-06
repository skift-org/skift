#pragma once

#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>
#include <libsystem/thread/Lock.h>
#include <libutils/RefCounted.h>
#include <libutils/ResultOr.h>

struct FsNode;
struct FsHandle;

typedef Result (*FsNodeOpenCallback)(struct FsNode *node, struct FsHandle *handle);
typedef void (*FsNodeCloseCallback)(struct FsNode *node, struct FsHandle *handle);

typedef struct FsNode *(*FsNodeFindCallback)(struct FsNode *node, const char *name);
typedef Result (*FsNodeLinkCallback)(struct FsNode *node, const char *name, struct FsNode *child);
typedef Result (*FsNodeUnlinkCallback)(struct FsNode *node, const char *name);

typedef Result (*FsNodeStatCallback)(struct FsNode *node, struct FsHandle *handle, FileState *stat);

typedef size_t (*FsNodeSizeCallback)(struct FsNode *node, struct FsHandle *handle);

typedef struct FsNode *(*FsNodeOpenConnectionCallback)(struct FsNode *node);

typedef void (*FsNodeAcceptCallback)(struct FsNode *node);
typedef bool (*FsNodeIsAcceptedCallback)(struct FsNode *node);

typedef bool (*FsNodeCanAcceptConnectionCallback)(struct FsNode *node);
typedef struct FsNode *(*FsNodeAcceptConnectionCallback)(struct FsNode *node);

struct FsNode : public RefCounted<FsNode>
{
    FileType type;
    Lock lock;

    uint readers = 0;
    uint writers = 0;
    uint clients = 0;
    uint server = 0;
    uint master = 0;

    FsNodeOpenCallback open = nullptr;
    FsNodeCloseCallback close = nullptr;
    FsNodeFindCallback find = nullptr;
    FsNodeLinkCallback link = nullptr;
    FsNodeUnlinkCallback unlink = nullptr;
    FsNodeStatCallback stat = nullptr;
    FsNodeSizeCallback size = nullptr;

    FsNodeOpenConnectionCallback open_connection = nullptr;
    FsNodeAcceptConnectionCallback accept_connection = nullptr;
    FsNodeCanAcceptConnectionCallback can_accept_connection = nullptr;

    FsNodeAcceptCallback accept = nullptr;
    FsNodeIsAcceptedCallback is_accepted = nullptr;

public:
    FsNode(FileType type);

    virtual ~FsNode();

    void ref_handle(FsHandle &handle);

    void deref_handle(FsHandle &handle);

    virtual Result call(FsHandle &handle, IOCall request, void *args)
    {
        __unused(handle);
        __unused(request);
        __unused(args);

        return ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }

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
