/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/tasking/Handles.h"
#include "kernel/node/Pipe.h"
#include "kernel/node/Terminal.h"
#include "kernel/sheduling/TaskBlockerSelect.h"

Result task_fshandle_add(Task *task, int *handle_index, FsHandle *handle)
{
    Result result = ERR_TOO_MANY_OPEN_FILES;

    lock_acquire(task->handles_lock);

    for (int i = 0; i < PROCESS_HANDLE_COUNT; i++)
    {
        if (task->handles[i] == NULL)
        {
            task->handles[i] = handle;
            *handle_index = i;

            result = SUCCESS;
            break;
        }
    }

    lock_release(task->handles_lock);

    return result;
}

Result task_fshandle_remove(Task *task, int handle_index)
{
    Result result = ERR_BAD_FILE_DESCRIPTOR;

    if (handle_index >= 0 && handle_index < PROCESS_HANDLE_COUNT)
    {
        lock_acquire(task->handles_lock);

        if (task->handles[handle_index] != NULL)
        {
            fshandle_destroy(task->handles[handle_index]);
            task->handles[handle_index] = NULL;

            result = SUCCESS;
        }

        lock_release(task->handles_lock);
    }
    else
    {
        logger_warn("Got a bad handle %d from task %d", handle_index, task->id);
    }

    return result;
}

FsHandle *task_fshandle_acquire(Task *task, int handle_index)
{
    FsHandle *result = NULL;

    if (handle_index >= 0 && handle_index < PROCESS_HANDLE_COUNT)
    {
        lock_acquire(task->handles_lock);

        if (task->handles[handle_index] != NULL)
        {
            fshandle_acquire_lock(task->handles[handle_index], task->id);
            result = task->handles[handle_index];
        }

        lock_release(task->handles_lock);
    }
    else
    {
        logger_warn("Got a bad handle %d from task %d", handle_index, task->id);
    }

    return result;
}

Result task_fshandle_release(Task *task, int handle_index)
{
    Result result = ERR_BAD_FILE_DESCRIPTOR;

    if (handle_index >= 0 && handle_index < PROCESS_HANDLE_COUNT)
    {
        lock_acquire(task->handles_lock);

        if (task->handles[handle_index] != NULL)
        {
            fshandle_release_lock(task->handles[handle_index], task->id);
            result = SUCCESS;
        }

        lock_release(task->handles_lock);
    }
    else
    {
        logger_warn("Got a bad handle %d from task %d", handle_index, task->id);
    }

    return result;
}

Result task_fshandle_open(Task *task, int *handle_index, const char *file_path, OpenFlag flags)
{
    Path *p = task_cwd_resolve(task, file_path);

    FsHandle *handle = NULL;
    Result result = filesystem_open(p, flags, &handle);

    path_destroy(p);

    if (handle == NULL)
    {
        *handle_index = HANDLE_INVALID_ID;
        return result;
    }

    result = task_fshandle_add(task, handle_index, handle);

    if (result != SUCCESS)
    {
        *handle_index = HANDLE_INVALID_ID;
        fshandle_destroy(handle);
    }

    return result;
}

void task_fshandle_close_all(Task *task)
{
    for (int i = 0; i < PROCESS_HANDLE_COUNT; i++)
    {
        task_fshandle_close(task, i);
    }
}

Result task_fshandle_close(Task *task, int handle_index)
{
    return task_fshandle_remove(task, handle_index);
}

Result task_fshandle_read(Task *task, int handle_index, void *buffer, size_t size, size_t *readed)
{
    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return ERR_BAD_FILE_DESCRIPTOR;
    }

    Result result = fshandle_read(handle, buffer, size, readed);

    task_fshandle_release(task, handle_index);

    return result;
}

Result task_fshandle_select(
    Task *task,
    HandleSet *handles_set,
    int *selected_index,
    SelectEvent *selected_events,
    Timeout timeout)
{
    Result result = SUCCESS;

    FsHandle *selected_handle = NULL;
    FsHandle **handles = calloc(handles_set->count, sizeof(FsHandle *));

    for (size_t i = 0; i < handles_set->count; i++)
    {
        handles[i] = task_fshandle_acquire(task, handles_set->handles[i]);

        if (handles[i] == NULL)
        {
            result = ERR_BAD_FILE_DESCRIPTOR;

            goto cleanup_and_return;
        }
    }

    TaskBlockerResult blocker_result = task_block(task, blocker_select_create(handles, handles_set->events, handles_set->count, &selected_handle, selected_events), timeout);

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

Result task_fshandle_write(Task *task, int handle_index, const void *buffer, size_t size, size_t *written)
{
    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return ERR_BAD_FILE_DESCRIPTOR;
    }

    Result result = fshandle_write(handle, buffer, size, written);

    task_fshandle_release(task, handle_index);

    return result;
}

Result task_fshandle_call(Task *task, int handle_index, int request, void *args)
{
    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return ERR_BAD_FILE_DESCRIPTOR;
    }

    Result result = fshandle_call(handle, request, args);

    task_fshandle_release(task, handle_index);

    return result;
}

Result task_fshandle_seek(Task *task, int handle_index, int offset, Whence whence)
{
    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return ERR_BAD_FILE_DESCRIPTOR;
    }

    Result result = fshandle_seek(handle, offset, whence);

    task_fshandle_release(task, handle_index);

    return result;
}

Result task_fshandle_tell(Task *task, int handle_index, Whence whence, int *offset)
{
    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return ERR_BAD_FILE_DESCRIPTOR;
    }

    Result result = fshandle_tell(handle, whence, offset);

    task_fshandle_release(task, handle_index);

    return result;
}

Result task_fshandle_stat(Task *task, int handle_index, FileState *stat)
{
    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return ERR_BAD_FILE_DESCRIPTOR;
    }

    Result result = fshandle_stat(handle, stat);

    task_fshandle_release(task, handle_index);

    return result;
}

Result task_fshandle_send(Task *task, int handle_index, Message *message)
{

    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return ERR_BAD_FILE_DESCRIPTOR;
    }

    Result result = fshandle_send(handle, message);

    task_fshandle_release(task, handle_index);

    return result;
}

Result task_fshandle_receive(Task *task, int handle_index, Message *message)
{
    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return ERR_BAD_FILE_DESCRIPTOR;
    }

    Result result = fshandle_receive(handle, message);

    task_fshandle_release(task, handle_index);

    return result;
}

Result task_fshandle_connect(Task *task, int *connection_handle_index, const char *socket_path)
{
    Path *resolved_path = task_cwd_resolve(task, socket_path);

    FsHandle *connection_handle;
    int result = filesystem_connect(resolved_path, &connection_handle);

    path_destroy(resolved_path);

    if (result != SUCCESS)
    {
        return result;
    }

    result = task_fshandle_add(task, connection_handle_index, connection_handle);

    if (result != SUCCESS)
    {
        fshandle_destroy(connection_handle);
    }

    return result;
}

Result task_fshandle_accept(Task *task, int socket_handle_index, int *connection_handle_index)
{
    FsHandle *socket_handle = task_fshandle_acquire(task, socket_handle_index);

    if (socket_handle == NULL)
    {
        return ERR_BAD_FILE_DESCRIPTOR;
    }

    FsHandle *connection_handle;
    Result result = fshandle_accept(socket_handle, &connection_handle);

    if (result == SUCCESS)
    {
        result = task_fshandle_add(task, connection_handle_index, connection_handle);

        if (result == SUCCESS)
        {
            result = task_fshandle_add(task, connection_handle_index, connection_handle);

            if (result != SUCCESS)
            {
                fshandle_destroy(connection_handle);
            }
        }
    }

    task_fshandle_release(task, socket_handle_index);

    return result;
}

Result task_fshandle_payload(Task *task, int handle_index, Message *message)
{
    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return ERR_BAD_FILE_DESCRIPTOR;
    }

    Result result = fshandle_payload(handle, message);

    task_fshandle_release(task, handle_index);

    return result;
}

Result task_fshandle_discard(Task *task, int handle_index)
{
    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return ERR_BAD_FILE_DESCRIPTOR;
    }

    Result result = fshandle_discard(handle);

    task_fshandle_release(task, handle_index);

    return result;
}

Result task_create_pipe(Task *task, int *reader_handle_index, int *writer_handle_index)
{
    *reader_handle_index = HANDLE_INVALID_ID;
    *writer_handle_index = HANDLE_INVALID_ID;

    Result result = SUCCESS;

    FsNode *pipe = pipe_create();

    FsHandle *reader_handle = fshandle_create(pipe, OPEN_READ);
    FsHandle *writer_handle = fshandle_create(pipe, OPEN_WRITE);

    result = task_fshandle_add(task, reader_handle_index, reader_handle);

    if (result != SUCCESS)
    {
        goto cleanup_and_return;
    }

    result = task_fshandle_add(task, writer_handle_index, writer_handle);

    if (result != SUCCESS)
    {
        goto cleanup_and_return;
    }

cleanup_and_return:
    if (result != SUCCESS)
    {
        if (*reader_handle_index != HANDLE_INVALID_ID)
        {
            task_fshandle_remove(task, *reader_handle_index);
            *reader_handle_index = HANDLE_INVALID_ID;
        }

        if (*writer_handle_index != HANDLE_INVALID_ID)
        {
            task_fshandle_remove(task, *writer_handle_index);
            *writer_handle_index = HANDLE_INVALID_ID;
        }

        fshandle_destroy(reader_handle);
        fshandle_destroy(writer_handle);
    }

    fsnode_deref(pipe);

    return result;
}

Result task_create_term(Task *task, int *master_handle_index, int *slave_handle_index)
{
    *master_handle_index = HANDLE_INVALID_ID;
    *slave_handle_index = HANDLE_INVALID_ID;

    Result result = SUCCESS;

    FsNode *terminal = terminal_create();

    FsHandle *master_handle = fshandle_create(terminal, OPEN_MASTER | OPEN_READ | OPEN_WRITE);
    FsHandle *slave_handle = fshandle_create(terminal, OPEN_READ | OPEN_WRITE);

    result = task_fshandle_add(task, master_handle_index, master_handle);

    if (result != SUCCESS)
    {
        goto cleanup_and_return;
    }

    result = task_fshandle_add(task, slave_handle_index, slave_handle);

    if (result != SUCCESS)
    {
        goto cleanup_and_return;
    }

cleanup_and_return:
    if (result != SUCCESS)
    {
        if (*master_handle_index != HANDLE_INVALID_ID)
        {
            task_fshandle_remove(task, *master_handle_index);
            *master_handle_index = HANDLE_INVALID_ID;
        }

        if (*slave_handle_index != HANDLE_INVALID_ID)
        {
            task_fshandle_remove(task, *slave_handle_index);
            *slave_handle_index = HANDLE_INVALID_ID;
        }

        fshandle_destroy(master_handle);
        fshandle_destroy(slave_handle);
    }

    fsnode_deref(terminal);

    return result;
}
