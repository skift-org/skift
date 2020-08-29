
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>

#include "kernel/filesystem/Filesystem.h"

FsNode::FsNode(FileType type)
{
    lock_init(lock);
    refcount = 1;
    this->type = type;
}

FsNode *FsNode::ref()
{
    __atomic_add_fetch(&refcount, 1, __ATOMIC_SEQ_CST);

    return this;
}

void FsNode::deref()
{
    if (__atomic_sub_fetch(&refcount, 1, __ATOMIC_SEQ_CST) == 0)
    {
        if (destroy)
        {
            destroy(this);
        }

        free(this);
    }
}

void FsNode::ref_handle(FsHandle &handle)
{
    if (handle.flags & OPEN_READ)
        __atomic_add_fetch(&this->readers, 1, __ATOMIC_SEQ_CST);

    if (handle.flags & OPEN_WRITE)
        __atomic_add_fetch(&this->writers, 1, __ATOMIC_SEQ_CST);

    if (handle.flags & OPEN_CLIENT)
        __atomic_add_fetch(&this->clients, 1, __ATOMIC_SEQ_CST);

    if (handle.flags & OPEN_SERVER)
        __atomic_add_fetch(&this->server, 1, __ATOMIC_SEQ_CST);

    if (handle.flags & OPEN_MASTER)
        __atomic_add_fetch(&this->master, 1, __ATOMIC_SEQ_CST);

    this->ref();
}

void FsNode::deref_handle(FsHandle &handle)
{
    if (handle.flags & OPEN_READ)
        __atomic_sub_fetch(&this->readers, 1, __ATOMIC_SEQ_CST);

    if (handle.flags & OPEN_WRITE)
        __atomic_sub_fetch(&this->writers, 1, __ATOMIC_SEQ_CST);

    if (handle.flags & OPEN_CLIENT)
        __atomic_sub_fetch(&this->clients, 1, __ATOMIC_SEQ_CST);

    if (handle.flags & OPEN_SERVER)
        __atomic_sub_fetch(&this->server, 1, __ATOMIC_SEQ_CST);

    if (handle.flags & OPEN_MASTER)
        __atomic_sub_fetch(&this->master, 1, __ATOMIC_SEQ_CST);

    this->deref();
}

bool fsnode_can_read(FsNode *node, FsHandle *handle)
{
    if (node->can_read)
    {
        return node->can_read(node, handle);
    }
    else
    {
        return true;
    }
}

bool fsnode_can_write(FsNode *node, FsHandle *handle)
{
    if (node->can_write)
    {
        return node->can_write(node, handle);
    }
    else
    {
        return true;
    }
}

bool fsnode_can_accept(FsNode *node)
{
    if (node->can_accept_connection)
    {
        return node->can_accept_connection(node);
    }
    else
    {
        return true;
    }
}

bool fsnode_is_accepted(FsNode *node)
{
    if (node->is_accepted)
    {
        return node->is_accepted(node);
    }
    else
    {
        return true;
    }
}

bool fsnode_is_acquire(FsNode *node)
{
    return lock_is_acquire(node->lock);
}

void fsnode_acquire_lock(FsNode *node, int who_acquire)
{
    lock_acquire_by(node->lock, who_acquire);
}

void fsnode_release_lock(FsNode *node, int who_release)
{
    lock_release_by(node->lock, who_release);
}
