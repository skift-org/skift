
#include <libsystem/Logger.h>

#include "kernel/filesystem/Filesystem.h"
#include "kernel/node/Pipe.h"
#include "kernel/node/Terminal.h"
#include "kernel/scheduling/Blocker.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/tasking/Task-Handles.h"

ResultOr<int> task_fshandle_add(Task *task, FsHandle *handle)
{
    LockHolder holder(task->handles_lock);

    for (int i = 0; i < PROCESS_HANDLE_COUNT; i++)
    {
        if (task->handles[i] == nullptr)
        {
            task->handles[i] = handle;

            return i;
        }
    }

    return ERR_TOO_MANY_HANDLE;
}

Result task_fshandle_add_at(Task *task, FsHandle *handle, int index)
{
    if (index < 0 && index >= PROCESS_HANDLE_COUNT)
    {
        return ERR_BAD_HANDLE;
    }

    LockHolder holder(task->handles_lock);

    if (task->handles[index])
    {
        delete task->handles[index];
        task->handles[index] = nullptr;
    }

    task->handles[index] = handle;

    return SUCCESS;
}

static bool is_valid_handle(Task *task, int handle)
{
    return handle >= 0 && handle < PROCESS_HANDLE_COUNT &&
           task->handles[handle] != nullptr;
}

Result task_fshandle_remove(Task *task, int handle_index)
{
    LockHolder holder(task->handles_lock);

    if (!is_valid_handle(task, handle_index))
    {
        logger_warn("Got a bad handle %d from task %d", handle_index, task->id);
        return ERR_BAD_HANDLE;
    }

    delete task->handles[handle_index];
    task->handles[handle_index] = nullptr;

    return SUCCESS;
}

FsHandle *task_fshandle_acquire(Task *task, int handle_index)
{
    LockHolder holder(task->handles_lock);

    if (!is_valid_handle(task, handle_index))
    {
        logger_warn("Got a bad handle %d from task %d", handle_index, task->id);
        return nullptr;
    }

    task->handles[handle_index]->acquire(task->id);
    return task->handles[handle_index];
}

Result task_fshandle_release(Task *task, int handle_index)
{
    LockHolder holder(task->handles_lock);

    if (!is_valid_handle(task, handle_index))
    {
        logger_warn("Got a bad handle %d from task %d", handle_index, task->id);
        return ERR_BAD_HANDLE;
    }

    task->handles[handle_index]->release(task->id);
    return SUCCESS;
}

ResultOr<int> task_fshandle_open(Task *task, Path &path, OpenFlag flags)
{
    auto result_or_handle = filesystem_open(path, flags);

    if (!result_or_handle.success())
    {
        return result_or_handle.result();
    }

    auto handle = result_or_handle.take_value();

    auto result_or_handle_index = task_fshandle_add(task, handle);

    if (!result_or_handle_index.success())
    {
        delete handle;
    }

    return result_or_handle_index;
}

Result task_fshandle_close(Task *task, int handle_index)
{
    return task_fshandle_remove(task, handle_index);
}

void task_fshandle_close_all(Task *task)
{
    LockHolder holder(task->handles_lock);

    for (int i = 0; i < PROCESS_HANDLE_COUNT; i++)
    {
        if (task->handles[i])
        {
            delete task->handles[i];
            task->handles[i] = nullptr;
        }
    }
}

Result task_fshandle_reopen(Task *task, int handle, int *reopened)
{
    auto original_handle = task_fshandle_acquire(task, handle);

    if (original_handle == nullptr)
    {
        return ERR_BAD_HANDLE;
    }

    auto reopened_handle = new FsHandle(*original_handle);

    auto result_or_handle_index = task_fshandle_add(task, reopened_handle);

    if (!result_or_handle_index.success())
    {
        delete reopened_handle;
        return result_or_handle_index.result();
    }

    *reopened = result_or_handle_index.take_value();

    return SUCCESS;
}

Result task_fshandle_copy(Task *task, int source, int destination)
{
    auto source_handle = task_fshandle_acquire(task, source);

    if (source_handle == nullptr)
    {
        return ERR_BAD_HANDLE;
    }

    auto copy_handle = new FsHandle(*source_handle);

    auto result = task_fshandle_add_at(task, copy_handle, destination);

    if (result != SUCCESS)
    {
        delete copy_handle;
    }

    return result;
}

Result task_fshandle_poll(
    Task *task,
    HandleSet *handles_set,
    int *selected_index,
    PollEvent *selected_events,
    Timeout timeout)
{
    Result result = SUCCESS;

    FsHandle *selected_handle = nullptr;
    FsHandle **handles = (FsHandle **)calloc(handles_set->count, sizeof(FsHandle *));

    for (size_t i = 0; i < handles_set->count; i++)
    {
        handles[i] = task_fshandle_acquire(task, handles_set->handles[i]);

        if (handles[i] == nullptr)
        {
            result = ERR_BAD_HANDLE;

            goto cleanup_and_return;
        }
    }

    {
        auto blocker = new BlockerSelect{
            handles,
            handles_set->events,
            handles_set->count,
            &selected_handle,
            selected_events,
        };

        BlockerResult blocker_result = task_block(task, blocker, timeout);

        if (blocker_result == BLOCKER_TIMEOUT)
        {
            result = TIMEOUT;
            goto cleanup_and_return;
        }

        if (selected_handle)
        {
            for (size_t i = 0; i < handles_set->count; i++)
            {
                if (handles[i] == selected_handle)
                {
                    *selected_index = handles_set->handles[i];

                    goto cleanup_and_return;
                }
            }
        }
    }

cleanup_and_return:

    if (handles)
    {
        for (size_t i = 0; i < handles_set->count; i++)
        {
            if (handles[i])
            {
                task_fshandle_release(task, handles_set->handles[i]);
            }
        }

        free(handles);
    }

    return result;
}

ResultOr<size_t> task_fshandle_read(Task *task, int handle_index, void *buffer, size_t size)
{
    auto handle = task_fshandle_acquire(task, handle_index);

    if (handle == nullptr)
    {
        return ERR_BAD_HANDLE;
    }

    auto result_or_read = handle->read(buffer, size);

    task_fshandle_release(task, handle_index);

    return result_or_read;
}

ResultOr<size_t> task_fshandle_write(Task *task, int handle_index, const void *buffer, size_t size)
{
    auto handle = task_fshandle_acquire(task, handle_index);

    if (handle == nullptr)
    {
        return ERR_BAD_HANDLE;
    }

    auto result_or_written = handle->write(buffer, size);

    task_fshandle_release(task, handle_index);

    return result_or_written;
}

ResultOr<int> task_fshandle_seek(Task *task, int handle_index, int offset, Whence whence)
{
    auto handle = task_fshandle_acquire(task, handle_index);

    if (handle == nullptr)
    {
        return ERR_BAD_HANDLE;
    }

    auto seek_result = handle->seek(offset, whence);

    task_fshandle_release(task, handle_index);

    return seek_result;
}

Result task_fshandle_call(Task *task, int handle_index, IOCall request, void *args)
{
    auto handle = task_fshandle_acquire(task, handle_index);

    if (handle == nullptr)
    {
        return ERR_BAD_HANDLE;
    }

    auto result = handle->call(request, args);

    task_fshandle_release(task, handle_index);

    return result;
}

Result task_fshandle_stat(Task *task, int handle_index, FileState *stat)
{
    auto handle = task_fshandle_acquire(task, handle_index);

    if (handle == nullptr)
    {
        return ERR_BAD_HANDLE;
    }

    auto result = handle->stat(stat);

    task_fshandle_release(task, handle_index);

    return result;
}

ResultOr<int> task_fshandle_connect(Task *task, Path &path)
{
    auto result_or_connection_handle = filesystem_connect(path);

    if (!result_or_connection_handle.success())
    {
        return result_or_connection_handle.result();
    }

    auto connection_handle = result_or_connection_handle.take_value();

    auto result_or_connection_handle_index = task_fshandle_add(task, connection_handle);

    if (!result_or_connection_handle_index.success())
    {
        delete connection_handle;
    }

    return result_or_connection_handle_index;
}

ResultOr<int> task_fshandle_accept(Task *task, int socket_handle_index)
{
    FsHandle *socket_handle = task_fshandle_acquire(task, socket_handle_index);

    if (socket_handle == nullptr)
    {
        return ERR_BAD_HANDLE;
    }

    auto result_or_connection_handle = socket_handle->accept();

    if (!result_or_connection_handle.success())
    {
        task_fshandle_release(task, socket_handle_index);

        return result_or_connection_handle.result();
    }

    auto connection_handle = result_or_connection_handle.take_value();

    auto result_or_connection_handle_index = task_fshandle_add(task, connection_handle);

    if (!result_or_connection_handle_index.success())
    {
        delete connection_handle;
    }

    task_fshandle_release(task, socket_handle_index);

    return result_or_connection_handle_index;
}

Result task_create_pipe(Task *task, int *reader_handle_index, int *writer_handle_index)
{
    *reader_handle_index = HANDLE_INVALID_ID;
    *writer_handle_index = HANDLE_INVALID_ID;

    auto pipe = make<FsPipe>();

    auto reader_handle = new FsHandle(pipe, OPEN_READ);
    auto writer_handle = new FsHandle(pipe, OPEN_WRITE);

    auto close_opened_handles = [&]() {
        if (*reader_handle_index != HANDLE_INVALID_ID)
        {
            task_fshandle_remove(task, *reader_handle_index);
        }

        if (reader_handle)
        {
            delete reader_handle;
        }

        if (*writer_handle_index != HANDLE_INVALID_ID)
        {
            task_fshandle_remove(task, *writer_handle_index);
        }

        if (writer_handle)
        {
            delete writer_handle;
        }
    };

    auto result_or_reader_handle_index = task_fshandle_add(task, reader_handle);

    if (!result_or_reader_handle_index.success())
    {
        close_opened_handles();
        return result_or_reader_handle_index.result();
    }
    else
    {
        *reader_handle_index = result_or_reader_handle_index.take_value();
    }

    auto result_or_writer_handle_index = task_fshandle_add(task, writer_handle);

    if (!result_or_writer_handle_index.success())
    {
        close_opened_handles();
        return result_or_writer_handle_index.result();
    }
    else
    {
        *writer_handle_index = result_or_writer_handle_index.take_value();
    }

    return SUCCESS;
}

Result task_create_term(Task *task, int *server_handle_index, int *client_handle_index)
{
    *server_handle_index = HANDLE_INVALID_ID;
    *client_handle_index = HANDLE_INVALID_ID;

    auto terminal = make<FsTerminal>();

    FsHandle *server_handle = new FsHandle(terminal, OPEN_SERVER);
    FsHandle *client_handle = new FsHandle(terminal, OPEN_READ | OPEN_WRITE);

    auto close_opened_handles = [&]() {
        if (*server_handle_index != HANDLE_INVALID_ID)
        {
            task_fshandle_remove(task, *server_handle_index);
        }

        if (server_handle)
        {
            delete server_handle;
        }

        if (*client_handle_index != HANDLE_INVALID_ID)
        {
            task_fshandle_remove(task, *client_handle_index);
        }

        if (client_handle)
        {
            delete client_handle;
        }
    };

    auto result_or_server_handle_index = task_fshandle_add(task, server_handle);

    if (!result_or_server_handle_index.success())
    {
        close_opened_handles();
        return result_or_server_handle_index.result();
    }
    else
    {
        *server_handle_index = result_or_server_handle_index.take_value();
    }

    auto result_or_client_handle_index = task_fshandle_add(task, client_handle);

    if (!result_or_client_handle_index.success())
    {
        close_opened_handles();
        return result_or_client_handle_index.result();
    }
    else
    {
        *client_handle_index = result_or_client_handle_index.take_value();
    }

    return SUCCESS;
}
