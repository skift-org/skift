#pragma once

#include <abi/Filesystem.h>
#include <abi/IOCall.h>

#include <libsystem/Result.h>
#include <libsystem/thread/Lock.h>
#include <libutils/RefPtr.h>
#include <libutils/ResultOr.h>
#include <libutils/String.h>

struct FsNode;
struct FsHandle;

struct FsNode : public RefCounted<FsNode>
{
private:
    Lock _lock{"fsnode"};
    FileType _type;

    unsigned int _readers = 0;
    unsigned int _writers = 0;
    unsigned int _clients = 0;
    unsigned int _server = 0;

public:
    FileType type() { return _type; }

    unsigned int readers() { return _readers; }

    unsigned int writers() { return _writers; }

    unsigned int clients() { return _clients; }

    unsigned int server() { return _server; }

    FsNode(FileType type);

    virtual ~FsNode()
    {
    }

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

    virtual RefPtr<FsNode> find(String name)
    {
        __unused(name);

        return nullptr;
    }

    virtual Result link(String name, RefPtr<FsNode> child)
    {
        __unused(name);
        __unused(child);

        return ERR_OPERATION_NOT_SUPPORTED;
    }

    virtual Result unlink(String name)
    {
        __unused(name);

        return ERR_OPERATION_NOT_SUPPORTED;
    }

    // Function called when the server accept the connection.
    virtual void accepted() {}

    // Return true if the connection is accepted
    virtual bool is_accepted() { return false; }

    virtual ResultOr<RefPtr<FsNode>> connect() { return ERR_SOCKET_OPERATION_ON_NON_SOCKET; }

    virtual bool can_accept() { return false; }

    virtual ResultOr<RefPtr<FsNode>> accept() { return ERR_SOCKET_OPERATION_ON_NON_SOCKET; }

    bool is_acquire();

    void acquire(int who_acquire);

    void release(int who_release);
};
