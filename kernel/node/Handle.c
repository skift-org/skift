/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libmath/MinMax.h>
#include <libsystem/Result.h>
#include <libsystem/assert.h>
#include <libsystem/cstring.h>

#include "kernel/node/Connection.h"
#include "kernel/node/Handle.h"
#include "kernel/sheduling/TaskBlockerAccept.h"
#include "kernel/sheduling/TaskBlockerConnect.h"
#include "kernel/sheduling/TaskBlockerRead.h"
#include "kernel/sheduling/TaskBlockerReceive.h"
#include "kernel/sheduling/TaskBlockerWrite.h"
#include "kernel/tasking.h"

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

SelectEvent fshandle_select(FsHandle *handle, SelectEvent events)
{
    FsNode *node = handle->node;
    SelectEvent selected_events = 0;

    if ((events & SELECT_READ) && fsnode_can_read(node, handle))
    {
        selected_events |= SELECT_READ;
    }

    if ((events & SELECT_WRITE) && fsnode_can_write(node, handle))
    {
        selected_events |= SELECT_WRITE;
    }

    if ((events & SELECT_SEND))
    {
        // FIXME: check if the message buffer is not full
        selected_events |= SELECT_SEND;
    }

    if ((events & SELECT_RECEIVE) && fsnode_can_receive(node, handle))
    {
        selected_events |= SELECT_RECEIVE;
    }

    if ((events & SELECT_CONNECT) && fsnode_is_accepted(node))
    {
        selected_events |= SELECT_CONNECT;
    }

    if ((events & SELECT_ACCEPT) && fsnode_can_accept(node))
    {
        selected_events |= SELECT_ACCEPT;
    }

    return selected_events;
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

Result fshandle_read(FsHandle *handle, void *buffer, size_t size, size_t *readed)
{
    if (!fshandle_has_flag(handle, OPEN_READ))
    {
        return ERR_WRITE_ONLY_STREAM;
    }

    FsNode *node = handle->node;

    if (!node->read)
    {
        return ERR_NOT_READABLE;
    }

    task_block(sheduler_running(), blocker_read_create(handle), 0);

    *readed = 0;

    Result result = node->read(node, handle, buffer, size, readed);

    handle->offset += *readed;

    fsnode_release_lock(node, sheduler_running_id());

    return result;
}

static Result fshandle_write_interal(FsHandle *handle, const char *buffer, size_t size, size_t *written)
{
    FsNode *node = handle->node;

    task_block(sheduler_running(), blocker_write_create(handle), 0);

    if (fshandle_has_flag(handle, OPEN_APPEND))
    {
        if (node->size)
        {
            handle->offset = node->size(node, handle);
        }
    }

    *written = 0;

    Result result = node->write(node, handle, buffer, size, written);

    handle->offset += *written;

    fsnode_release_lock(node, sheduler_running_id());

    return result;
}

Result fshandle_write(FsHandle *handle, const void *buffer, size_t size, size_t *written)
{
    int remaining = size;
    Result result = SUCCESS;
    size_t written_this_time = 0;

    *written = 0;

    if (!fshandle_has_flag(handle, OPEN_WRITE))
    {
        return ERR_READ_ONLY_STREAM;
    }

    if (!handle->node->write)
    {
        return ERR_NOT_WRITABLE;
    }

    while (remaining > 0 && result == SUCCESS)
    {
        result = fshandle_write_interal(
            handle,
            (void *)((uintptr_t)buffer + (size - remaining)),
            remaining,
            &written_this_time);

        remaining -= written_this_time;
    }

    *written = size - remaining;
    return result;
}

Result fshandle_seek(FsHandle *handle, int offset, Whence whence)
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
        handle->offset = MAX(0, offset);
        break;

    case WHENCE_HERE:
        handle->offset = handle->offset + offset;
        break;

    case WHENCE_END:
        if (offset < 0)
        {
            if ((size_t)-offset <= size)
            {
                handle->offset = size + offset;
            }
            else
            {
                handle->offset = 0;
            }
        }
        else
        {
            handle->offset = size + offset;
        }

        break;
    default:
        logger_error("Invalide whence %d", whence);
        ASSERT_NOT_REACHED();
    }

    return SUCCESS;
}

Result fshandle_tell(FsHandle *handle, Whence whence, int *offset)
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
        *offset = handle->offset;
        break;

    case WHENCE_END:
        *offset = handle->offset - size;
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    return SUCCESS;
}

Result fshandle_call(FsHandle *handle, int request, void *args)
{
    Result result = ERR_OPERATION_NOT_SUPPORTED;

    FsNode *node = handle->node;

    if (node->call)
    {
        fsnode_acquire_lock(node, sheduler_running_id());
        result = node->call(node, handle, request, args);
        fsnode_release_lock(node, sheduler_running_id());
    }

    return result;
}

Result fshandle_stat(FsHandle *handle, FileState *stat)
{
    int result = SUCCESS;

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

Result fshandle_connect(FsNode *node, FsHandle **connection_handle)
{
    if (!node->open_connection)
    {
        return ERR_SOCKET_OPERATION_ON_NON_SOCKET;
    }

    fsnode_acquire_lock(node, sheduler_running_id());

    FsNode *connection = node->open_connection(node);

    fsnode_release_lock(node, sheduler_running_id());

    if (connection == NULL)
    {
        return ERR_CONNECTION_REFUSED;
    }

    task_block(sheduler_running(), blocker_connect_create(connection), 0);

    *connection_handle = fshandle_create(connection, OPEN_CLIENT);
    fsnode_deref(connection);

    return SUCCESS;
}

Result fshandle_accept(FsHandle *handle, FsHandle **connection_handle)
{
    FsNode *node = handle->node;

    if (!node->accept_connection)
    {
        return ERR_SOCKET_OPERATION_ON_NON_SOCKET;
    }

    task_block(sheduler_running(), blocker_accept_create(node), 0);

    FsNode *connection = node->accept_connection(node);

    fsnode_release_lock(node, sheduler_running_id());

    *connection_handle = fshandle_create(connection, OPEN_SERVER);

    fsnode_deref(connection);

    return SUCCESS;
}

Result fshandle_send(FsHandle *handle, Message *message)
{
    FsNode *node = handle->node;

    if (!node->send)
    {
        return ERR_SOCKET_OPERATION_ON_NON_SOCKET;
    }

    fsnode_acquire_lock(node, sheduler_running_id());

    Result result = node->send(node, handle, message);

    fsnode_release_lock(node, sheduler_running_id());

    return result;
}

Result fshandle_receive(FsHandle *handle, Message *message)
{
    FsNode *node = handle->node;

    if (!node->receive)
    {
        return ERR_SOCKET_OPERATION_ON_NON_SOCKET;
    }

    task_block(sheduler_running(), blocker_receive_create(handle), 0);

    if (handle->message != NULL)
    {
        free(handle->message);
        handle->message = NULL;
    }

    Result result = node->receive(node, handle, &handle->message);

    if (handle->message != NULL)
    {
        memcpy(message, handle->message, sizeof(Message));
    }

    fsnode_release_lock(node, sheduler_running_id());

    return result;
}

Result fshandle_payload(FsHandle *handle, Message *message)
{
    if (handle->message == NULL)
    {
        return ERR_NO_MESSAGE;
    }

    memcpy(message, handle->message, handle->message->size);

    return SUCCESS;
}

Result fshandle_discard(FsHandle *handle)
{
    if (handle->message != NULL)
    {
        free(handle->message);
        handle->message = NULL;
    }

    return SUCCESS;
}