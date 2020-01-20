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

FsHandle *fshandle_create(FsNode *node, OpenFlag flags)
{
    FsHandle *handle = __create(FsHandle);

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

FsHandle *fshandle_clone(FsHandle *handle)
{
    FsHandle *clone = __create(FsHandle);
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

bool fshandle_has_flag(FsHandle *handle, OpenFlag flag)
{
    return (handle->flags & flag) == flag;
}

void fshandle_destroy(FsHandle *handle)
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

bool fshandle_is_locked(FsHandle *handle)
{
    return lock_is_acquire(handle->lock);
}

void fshandle_acquire_lock(FsHandle *handle, int who_acquire)
{
    lock_acquire_by(handle->lock, who_acquire);
}

void fshandle_release_lock(FsHandle *handle, int who_release)
{
    lock_release_by(handle->lock, who_release);
}

int fshandle_read(FsHandle *handle, void *buffer, size_t size)
{
    if (!fshandle_has_flag(handle, OPEN_READ))
    {
        return -ERR_WRITE_ONLY_STREAM;
    }

    FsNode *node = handle->node;

    if (!node->read)
    {
        return -ERR_NOT_READABLE;
    }

    task_block(sheduler_running(), blocker_read_create(node));

    int readded = node->read(node, handle, buffer, size);

    if (readded > 0)
    {
        handle->offset += readded;
    }

    fsnode_release_lock(node, sheduler_running_id());

    return readded;
}

int fshandle_write(FsHandle *handle, const void *buffer, size_t size)
{
    if (!fshandle_has_flag(handle, OPEN_WRITE))
    {
        return ERR_READ_ONLY_STREAM;
    }

    FsNode *node = handle->node;

    if (!node->write)
    {
        return -ERR_NOT_WRITABLE;
    }

    int written = 0;
    int remaining = size;

    while (remaining > 0)
    {
        task_block(sheduler_running(), blocker_write_create(node));

        if (fshandle_has_flag(handle, OPEN_APPEND))
        {
            if (node->size)
            {
                handle->offset = node->size(node, handle);
            }
        }

        int result = node->write(node, handle, (void *)((uintptr_t)buffer + written), remaining);

        if (result <= 0)
        {
            remaining = 0;
            written = result;
        }
        else
        {
            written += result;
            remaining -= result;
            handle->offset += result;
        }

        fsnode_release_lock(node, sheduler_running_id());
    }

    return written;
}

off_t fshandle_seek(FsHandle *handle, Whence whence, off_t where)
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

off_t fshandle_tell(FsHandle *handle, Whence whence)
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

int fshandle_call(FsHandle *handle, int request, void *args)
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

int fshandle_stat(FsHandle *handle, FileState *stat)
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
