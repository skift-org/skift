/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libmath/math.h>
#include <libsystem/assert.h>
#include <libsystem/error.h>

#include "node/Handle.h"
#include "sheduling/TaskBlockerRead.h"
#include "sheduling/TaskBlockerWrite.h"
#include "tasking.h"

Handle *handle_create(FsNode *node, OpenFlag flags)
{
    Handle *handle = __create(Handle);

    lock_init(handle->lock);

    handle->node = fsnode_ref(node);
    handle->offset = 0;
    handle->flags = flags;

    if (node->open)
    {
        fsnode_acquire_lock(node, sheduler_running_id());
        node->open(node, handle);
        fsnode_release_lock(node, sheduler_running_id());
    }

    return handle;
}

Handle *handle_clone(Handle *handle)
{
    Handle *clone = __create(Handle);
    FsNode *node = handle->node;

    lock_init(clone->lock);

    clone->node = fsnode_ref(node);
    clone->offset = handle->offset;
    clone->flags = handle->flags;

    if (node->open)
    {
        fsnode_acquire_lock(node, sheduler_running_id());
        node->open(node, clone);
        fsnode_release_lock(node, sheduler_running_id());
    }

    return clone;
}

bool handle_has_flag(Handle *handle, OpenFlag flag)
{
    return (handle->flags & flag) == flag;
}

void handle_destroy(Handle *handle)
{
    FsNode *node = handle->node;

    if (node->close)
    {
        fsnode_acquire_lock(node, sheduler_running_id());
        node->close(node, handle);
        fsnode_release_lock(node, sheduler_running_id());
    }

    fsnode_deref(node);
    free(handle);
}

bool handle_is_locked(Handle *handle)
{
    return lock_is_acquire(handle->lock);
}

void handle_acquire_lock(Handle *handle, int who_acquire)
{
    lock_acquire_by(handle->lock, who_acquire);
}

void handle_release_lock(Handle *handle, int who_release)
{
    lock_release_by(handle->lock, who_release);
}

int handle_read(Handle *handle, void *buffer, size_t size)
{
    if (handle_has_flag(handle, OPEN_READ))
    {
        FsNode *node = handle->node;

        if (node->read)
        {
            task_block(sheduler_running(), blocker_read_create(node));

            int readded = node->read(node, handle, buffer, size);

            if (readded > 0)
            {
                handle->offset += readded;
            }

            fsnode_release_lock(node, sheduler_running_id());

            return readded;
        }
        else
        {
            return -ERR_NOT_READABLE;
        }
    }
    else
    {
        return -ERR_WRITE_ONLY_STREAM;
    }
}

int handle_write(Handle *handle, const void *buffer, size_t size)
{
    if (handle_has_flag(handle, OPEN_WRITE))
    {
        FsNode *node = handle->node;

        if (node->write)
        {
            task_block(sheduler_running(), blocker_write_create(node));

            if (handle_has_flag(handle, OPEN_APPEND))
            {
                if (node->size)
                {
                    handle->offset = node->size(node, handle);
                }
            }

            int written = node->write(node, handle, buffer, size);

            if (written > 0)
            {
                handle->offset += written;
            }

            fsnode_release_lock(node, sheduler_running_id());

            return written;
        }
        else
        {
            return -ERR_NOT_WRITABLE;
        }
    }
    else
    {
        return ERR_READ_ONLY_STREAM;
    }
}

off_t handle_seek(Handle *handle, Whence whence, off_t where)
{
    FsNode *node = handle->node;

    size_t size = 0;

    if (node->size)
    {
        fsnode_acquire_lock(node, sheduler_running_id());
        size = node->size(node, handle);
        fsnode_release_lock(node, sheduler_running_id());
    }

    switch (whence)
    {
    case WHENCE_START:
        handle->offset = max(0, where);
        break;

    case WHENCE_HERE:
        handle->offset = handle->offset + where;
        break;

    case WHENCE_END:
        if (where < 0)
        {
            if ((size_t)-where <= size)
            {
                handle->offset = size + where;
            }
            else
            {
                handle->offset = 0;
            }
        }
        else
        {
            handle->offset = size + where;
        }

        break;
    default:
        logger_error("Invalide whence %d", whence);
        ASSERT_NOT_REACHED();
    }

    return -ERR_SUCCESS;
}

off_t handle_tell(Handle *handle, Whence whence)
{
    FsNode *node = handle->node;

    size_t size = 0;

    if (node->size)
    {
        fsnode_acquire_lock(node, sheduler_running_id());
        size = node->size(node, handle);
        fsnode_release_lock(node, sheduler_running_id());
    }

    switch (whence)
    {
    case WHENCE_START:
    case WHENCE_HERE:
        return handle->offset;

    case WHENCE_END:
        return handle->offset - size;
    default:
        ASSERT_NOT_REACHED();
    }
}

int handle_call(Handle *handle, int request, void *args)
{
    int result = -ERR_OPERATION_NOT_SUPPORTED;

    FsNode *node = handle->node;

    if (node->call)
    {
        fsnode_acquire_lock(node, sheduler_running_id());
        result = node->call(node, handle, request, args);
        fsnode_release_lock(node, sheduler_running_id());
    }

    return result;
}

int handle_stat(Handle *handle, FileState *stat)
{
    int result = -ERR_SUCCESS;

    FsNode *node = handle->node;

    if (node->size)
    {
        fsnode_acquire_lock(node, sheduler_running_id());
        stat->size = node->size(node, handle);
        fsnode_release_lock(node, sheduler_running_id());
    }
    else
    {
        stat->size = 0;
    }

    stat->type = node->type;

    if (node->stat)
    {
        fsnode_acquire_lock(node, sheduler_running_id());
        result = node->stat(node, handle, stat);
        fsnode_release_lock(node, sheduler_running_id());
    }

    return result;
}
