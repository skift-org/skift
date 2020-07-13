
#include <libsystem/CString.h>
#include <libsystem/Logger.h>

#include "kernel/filesystem/Filesystem.h"

void fsnode_init(FsNode *node, FileType type)
{
    lock_init(node->lock);
    node->refcount = 1;
    node->type = type;
}

FsNode *fsnode_ref(FsNode *node)
{
    __atomic_add_fetch(&node->refcount, 1, __ATOMIC_SEQ_CST);

    return node;
}

void fsnode_deref(FsNode *node)
{
    if (__atomic_sub_fetch(&node->refcount, 1, __ATOMIC_SEQ_CST) == 0)
    {
        if (node->destroy)
        {
            node->destroy(node);
        }

        free(node);
    }
}

FsNode *fsnode_ref_handle(FsNode *node, OpenFlag flags)
{
    if (flags & OPEN_READ)
        __atomic_add_fetch(&node->readers, 1, __ATOMIC_SEQ_CST);

    if (flags & OPEN_WRITE)
        __atomic_add_fetch(&node->writers, 1, __ATOMIC_SEQ_CST);

    if (flags & OPEN_CLIENT)
        __atomic_add_fetch(&node->clients, 1, __ATOMIC_SEQ_CST);

    if (flags & OPEN_SERVER)
        __atomic_add_fetch(&node->server, 1, __ATOMIC_SEQ_CST);

    if (flags & OPEN_MASTER)
        __atomic_add_fetch(&node->master, 1, __ATOMIC_SEQ_CST);

    return fsnode_ref(node);
}

void fsnode_deref_handle(FsNode *node, OpenFlag flags)
{
    if (flags & OPEN_READ)
        __atomic_sub_fetch(&node->readers, 1, __ATOMIC_SEQ_CST);

    if (flags & OPEN_WRITE)
        __atomic_sub_fetch(&node->writers, 1, __ATOMIC_SEQ_CST);

    if (flags & OPEN_CLIENT)
        __atomic_sub_fetch(&node->clients, 1, __ATOMIC_SEQ_CST);

    if (flags & OPEN_SERVER)
        __atomic_sub_fetch(&node->server, 1, __ATOMIC_SEQ_CST);

    if (flags & OPEN_MASTER)
        __atomic_sub_fetch(&node->master, 1, __ATOMIC_SEQ_CST);

    fsnode_deref(node);
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
