#pragma once

#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>
#include <libsystem/thread/Lock.h>
#include <libutils/RefCounted.h>
#include <libutils/ResultOr.h>

struct FsNode;
struct FsHandle;

struct FsNode : public RefCounted<FsNode>
{
    FileType type;
    Lock _lock;

    uint readers = 0;
    uint writers = 0;
    uint clients = 0;
    uint server = 0;
    uint master = 0;

public:
    FsNode(FileType type);

    virtual ~FsNode();

    void ref_handle(FsHandle &handle);

    void deref_handle(FsHandle &handle);

    virtual Result open(FsHandle *handle)
    {
        __unused(handle);
        return SUCCESS;
    }

    virtual void close(FsHandle *handle)
    {
        __unused(handle);
    }

    virtual size_t size()
    {
        return 0;
    }

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

    virtual FsNode *find(const char *name)
    {
        __unused(name);

        return nullptr;
    }

    virtual Result link(const char *name, FsNode *child)
    {
        __unused(name);
        __unused(child);

        return ERR_OPERATION_NOT_SUPPORTED;
    }

    virtual Result unlink(const char *name)
    {
        __unused(name);

        return ERR_OPERATION_NOT_SUPPORTED;
    }

    // Function called when the server accept the connection.
    virtual void accepted() {}

    // Return true if the connection is accepted
    virtual bool is_accepted() { return false; }

    virtual ResultOr<FsNode *> connect() { return ERR_SOCKET_OPERATION_ON_NON_SOCKET; }

    virtual bool can_accept() { return false; }

    virtual ResultOr<FsNode *> accept() { return ERR_SOCKET_OPERATION_ON_NON_SOCKET; }

    bool is_acquire();

    void acquire(int who_acquire);

    void release(int who_release);
};
