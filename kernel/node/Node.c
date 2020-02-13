/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libmath/math.h>
#include <libsystem/cstring.h>
#include <libsystem/logger.h>

#include "kernel/filesystem/Filesystem.h"

void fsnode_init(FsNode *node, FsNodeType type)
{
    lock_init(node->lock);
    node->refcount = 1;
    node->type = type;
}

FsNode *fsnode_ref(FsNode *node)
{
    node->refcount++;
    return node;
}

void fsnode_deref(FsNode *node)
{
    node->refcount--;

    if (node->refcount == 0)
    {
        if (node->destroy)
        {
            node->destroy(node);
        }

        free(node);
    }
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

bool fsnode_can_receive(FsNode *node, FsHandle *handle)
{
    if (node->can_receive)
    {
        return node->can_receive(node, handle);
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
