
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/core/CString.h>
#include <libsystem/math/MinMax.h>

#include "kernel/node/Connection.h"
#include "kernel/node/Handle.h"
#include "kernel/scheduling/Blocker.h"
#include "kernel/scheduling/Scheduler.h"

FsHandle *fshandle_create(FsNode *node, OpenFlag flags)
{
    FsHandle *handle = __create(FsHandle);

    lock_init(handle->lock);

    handle->offset = 0;
    handle->flags = flags;
    handle->node = node;

    node->acquire(scheduler_running_id());
    node->ref_handle(*handle);
    node->open(handle);
    node->release(scheduler_running_id());

    return handle;
}

FsHandle *fshandle_clone(FsHandle *handle)
{
    FsHandle *clone = __create(FsHandle);
    FsNode *node = handle->node;

    lock_init(clone->lock);

    clone->offset = handle->offset;
    clone->flags = handle->flags;
    node->ref_handle(*handle);
    clone->node = node;

    node->acquire(scheduler_running_id());
    node->open(clone);
    node->release(scheduler_running_id());

    return clone;
}

void fshandle_destroy(FsHandle *handle)
{
    FsNode *node = handle->node;

    node->acquire(scheduler_running_id());
    node->close(handle);
    node->deref_handle(*handle);
    node->release(scheduler_running_id());

    free(handle);
}

SelectEvent fshandle_select(FsHandle *handle, SelectEvent events)
{
    FsNode *node = handle->node;
    SelectEvent selected_events = 0;

    if ((events & SELECT_READ) && node->can_read(handle))
    {
        selected_events |= SELECT_READ;
    }

    if ((events & SELECT_WRITE) && node->can_write(handle))
    {
        selected_events |= SELECT_WRITE;
    }

    if ((events & SELECT_CONNECT) && node->is_accepted())
    {
        selected_events |= SELECT_CONNECT;
    }

    if ((events & SELECT_ACCEPT) && node->can_accept())
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

Result fshandle_read(FsHandle *handle, void *buffer, size_t size, size_t *read)
{
    if (!handle->has_flag(OPEN_READ) &&
        !handle->has_flag(OPEN_MASTER) &&
        !handle->has_flag(OPEN_SERVER) &&
        !handle->has_flag(OPEN_CLIENT))
    {
        return ERR_WRITE_ONLY_STREAM;
    }

    FsNode *node = handle->node;

    task_block(scheduler_running(), new BlockerRead(handle), -1);

    auto result_or_read = node->read(*handle, buffer, size);

    if (result_or_read.success())
    {
        handle->offset += result_or_read.value();
        *read = result_or_read.value();
    }
    else
    {
        *read = 0;
    }

    node->release(scheduler_running_id());

    return result_or_read.result();
}

static Result fshandle_write_internal(FsHandle *handle, const void *buffer, size_t size, size_t *written)
{
    FsNode *node = handle->node;

    task_block(scheduler_running(), new BlockerWrite(handle), -1);

    if (handle->has_flag(OPEN_APPEND))
    {
        handle->offset = node->size();
    }

    auto result_or_written = node->write(*handle, buffer, size);

    if (result_or_written.success())
    {
        handle->offset += result_or_written.value();
        *written = result_or_written.value();
    }
    else
    {
        *written = 0;
    }

    node->release(scheduler_running_id());

    return result_or_written.result();
}

Result fshandle_write(FsHandle *handle, const void *buffer, size_t size, size_t *written)
{
    int remaining = size;
    Result result = SUCCESS;
    size_t written_this_time = 0;

    *written = 0;

    if (!handle->has_flag(OPEN_WRITE) &&
        !handle->has_flag(OPEN_MASTER) &&
        !handle->has_flag(OPEN_SERVER) &&
        !handle->has_flag(OPEN_CLIENT))
    {
        return ERR_READ_ONLY_STREAM;
    }

    while (remaining > 0 && result == SUCCESS)
    {
        result = fshandle_write_internal(
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

    node->acquire(scheduler_running_id());
    size_t size = node->size();
    node->release(scheduler_running_id());

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
        logger_error("Invalid whence %d", whence);
        ASSERT_NOT_REACHED();
    }

    return SUCCESS;
}

Result fshandle_tell(FsHandle *handle, Whence whence, int *offset)
{
    FsNode *node = handle->node;

    size_t size = 0;

    node->acquire(scheduler_running_id());
    size = node->size();
    node->release(scheduler_running_id());

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

Result fshandle_call(FsHandle *handle, IOCall request, void *args)
{
    Result result = ERR_OPERATION_NOT_SUPPORTED;

    FsNode *node = handle->node;

    node->acquire(scheduler_running_id());
    result = node->call(*handle, request, args);
    node->release(scheduler_running_id());

    return result;
}

Result fshandle_stat(FsHandle *handle, FileState *stat)
{
    Result result = SUCCESS;

    FsNode *node = handle->node;

    node->acquire(scheduler_running_id());
    stat->size = node->size();
    stat->type = node->type;
    node->release(scheduler_running_id());

    return result;
}

Result fshandle_connect(FsNode *node, FsHandle **connection_handle)
{
    node->acquire(scheduler_running_id());
    auto connection_or_result = node->connect();
    node->release(scheduler_running_id());

    if (!connection_or_result.success())
    {
        return connection_or_result.result();
    }

    // We need to create the fshandle before releasing the lock
    // Because this will increment FsNode::clients

    auto connection = connection_or_result.take_value();

    *connection_handle = fshandle_create(connection, OPEN_CLIENT);

    task_block(scheduler_running(), new BlockerConnect(connection), -1);

    connection->deref();

    return SUCCESS;
}

Result fshandle_accept(FsHandle *handle, FsHandle **connection_handle)
{
    FsNode *node = handle->node;

    task_block(scheduler_running(), new BlockerAccept(node), -1);

    auto connection_or_result = node->accept();

    if (!connection_or_result.success())
    {
        node->release(scheduler_running_id());
        return connection_or_result.result();
    }

    // We need to create the fshandle before releasing the lock
    // Because this will increment FsNode::servers

    auto connection = connection_or_result.take_value();

    *connection_handle = fshandle_create(connection, OPEN_SERVER);

    node->release(scheduler_running_id());

    connection->deref();

    return SUCCESS;
}
