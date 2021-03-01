#include <libsystem/Logger.h>
#include <string.h>

#include "kernel/node/Handle.h"
#include "kernel/node/Node.h"

FsNode::FsNode(FileType type)
{
    _type = type;
}

void FsNode::ref_handle(FsHandle &handle)
{
    if (handle.flags() & OPEN_READ)
    {
        __atomic_add_fetch(&_readers, 1, __ATOMIC_SEQ_CST);
    }

    if (handle.flags() & OPEN_WRITE)
    {
        __atomic_add_fetch(&_writers, 1, __ATOMIC_SEQ_CST);
    }

    if (handle.flags() & OPEN_CLIENT)
    {
        __atomic_add_fetch(&_clients, 1, __ATOMIC_SEQ_CST);
    }

    if (handle.flags() & OPEN_SERVER)
    {
        __atomic_add_fetch(&_server, 1, __ATOMIC_SEQ_CST);
    }
}

void FsNode::deref_handle(FsHandle &handle)
{
    if (handle.flags() & OPEN_READ)
    {
        __atomic_sub_fetch(&_readers, 1, __ATOMIC_SEQ_CST);
    }

    if (handle.flags() & OPEN_WRITE)
    {
        __atomic_sub_fetch(&_writers, 1, __ATOMIC_SEQ_CST);
    }

    if (handle.flags() & OPEN_CLIENT)
    {
        __atomic_sub_fetch(&_clients, 1, __ATOMIC_SEQ_CST);
    }

    if (handle.flags() & OPEN_SERVER)
    {
        __atomic_sub_fetch(&_server, 1, __ATOMIC_SEQ_CST);
    }
}

bool FsNode::is_acquire()
{
    return _lock.locked();
}

void FsNode::acquire(int who_acquire)
{
    _lock.acquire_for(who_acquire, SOURCE_LOCATION);
}

void FsNode::release(int who_release)
{
    _lock.release_for(who_release, SOURCE_LOCATION);
}
