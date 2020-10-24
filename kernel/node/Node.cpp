
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>

#include "kernel/filesystem/Filesystem.h"

FsNode::FsNode(FileType type)
{
    lock_init(_lock);
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
    return lock_is_acquire(_lock);
}

void FsNode::acquire(int who_acquire)
{
    lock_acquire_by(_lock, who_acquire);
}

void FsNode::release(int who_release)
{
    lock_release_by(_lock, who_release);
}
