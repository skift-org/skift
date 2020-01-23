#include "tasking.h"

error_t task_fshandle_add(Task *task, int *handle_index, FsHandle *handle)
{
    error_t result = ERR_TOO_MANY_OPEN_FILES;

    lock_acquire(task->handles_lock);

    for (int i = 0; i < TASK_FILDES_COUNT; i++)
    {
        if (task->handles[i] == NULL)
        {
            task->handles[i] = handle;
            *handle_index = i;

            result = ERR_SUCCESS;
            break;
        }
    }

    lock_release(task->handles_lock);

    return result;
}

error_t task_fshandle_remove(Task *task, int handle_index)
{
    error_t result = ERR_BAD_FILE_DESCRIPTOR;

    if (handle_index >= 0 && handle_index < TASK_FILDES_COUNT)
    {
        lock_acquire(task->handles_lock);

        if (task->handles[handle_index] != NULL)
        {
            fshandle_destroy(task->handles[handle_index]);
            task->handles[handle_index] = NULL;

            result = ERR_SUCCESS;
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

    if (handle_index >= 0 && handle_index < TASK_FILDES_COUNT)
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

error_t task_fshandle_release(Task *task, int handle_index)
{
    error_t result = ERR_BAD_FILE_DESCRIPTOR;

    if (handle_index >= 0 && handle_index < TASK_FILDES_COUNT)
    {
        lock_acquire(task->handles_lock);

        if (task->handles[handle_index] != NULL)
        {
            fshandle_release_lock(task->handles[handle_index], task->id);
            result = ERR_SUCCESS;
        }

        lock_release(task->handles_lock);
    }
    else
    {
        logger_warn("Got a bad handle %d from task %d", handle_index, task->id);
    }

    return result;
}

error_t task_fshandle_open(Task *task, int *handle_index, const char *file_path, OpenFlag flags)
{
    Path *p = task_cwd_resolve(task, file_path);

    FsHandle *handle = filesystem_open(p, flags);

    path_delete(p);

    if (handle == NULL)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    error_t result = task_fshandle_add(task, handle_index, handle);

    if (result != ERR_SUCCESS)
    {
        fshandle_destroy(handle);
    }

    return result;
}

void task_fshandle_close_all(Task *task)
{
    for (int i = 0; i < TASK_FILDES_COUNT; i++)
    {
        task_fshandle_close(task, i);
    }
}

error_t task_fshandle_close(Task *task, int handle_index)
{
    return task_fshandle_remove(task, handle_index);
}

int task_fshandle_read(Task *task, int handle_index, void *buffer, uint size)
{
    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return -ERR_BAD_FILE_DESCRIPTOR;
    }

    int result = fshandle_read(handle, buffer, size);

    task_fshandle_release(task, handle_index);

    return result;
}

int task_fshandle_write(Task *task, int handle_index, const void *buffer, uint size)
{
    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return -ERR_BAD_FILE_DESCRIPTOR;
    }

    int result = fshandle_write(handle, buffer, size);

    task_fshandle_release(task, handle_index);

    return result;
}

int task_fshandle_call(Task *task, int handle_index, int request, void *args)
{
    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return -ERR_BAD_FILE_DESCRIPTOR;
    }

    int result = fshandle_call(handle, request, args);

    task_fshandle_release(task, handle_index);

    return result;
}

int task_fshandle_seek(Task *task, int handle_index, Whence whence, off_t offset)
{
    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return -ERR_BAD_FILE_DESCRIPTOR;
    }

    int result = fshandle_seek(handle, whence, offset);

    task_fshandle_release(task, handle_index);

    return result;
}

int task_fshandle_tell(Task *task, int handle_index, Whence whence)
{
    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return -ERR_BAD_FILE_DESCRIPTOR;
    }

    int result = fshandle_tell(handle, whence);

    task_fshandle_release(task, handle_index);

    return result;
}

int task_fshandle_stat(Task *task, int handle_index, FileState *stat)
{
    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return -ERR_BAD_FILE_DESCRIPTOR;
    }

    int result = fshandle_stat(handle, stat);

    task_fshandle_release(task, handle_index);

    return result;
}

error_t task_fshandle_send(Task *task, int handle_index, Message *message)
{

    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return ERR_BAD_FILE_DESCRIPTOR;
    }

    error_t result = fshandle_send(handle, message);

    task_fshandle_release(task, handle_index);

    return result;
}

error_t task_fshandle_receive(Task *task, int handle_index, Message *message)
{
    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return ERR_BAD_FILE_DESCRIPTOR;
    }

    error_t result = fshandle_receive(handle, message);

    task_fshandle_release(task, handle_index);

    return result;
}

error_t task_fshandle_connect(Task *task, int *connection_handle_index, const char *socket_path)
{
    Path *resolved_path = task_cwd_resolve(task, socket_path);

    FsHandle *connection_handle;
    int result = filesystem_connect(resolved_path, &connection_handle);

    path_delete(resolved_path);

    if (result != ERR_SUCCESS)
    {
        return result;
    }

    result = task_fshandle_add(task, connection_handle_index, connection_handle);

    if (result != ERR_SUCCESS)
    {
        fshandle_destroy(connection_handle);
    }

    return result;
}

error_t task_fshandle_accept(Task *task, int socket_handle_index, int *connection_handle_index)
{
    FsHandle *socket_handle = task_fshandle_acquire(task, socket_handle_index);

    if (socket_handle == NULL)
    {
        return ERR_BAD_FILE_DESCRIPTOR;
    }

    FsHandle *connection_handle;
    error_t result = fshandle_accept(socket_handle, &connection_handle);

    if (result == ERR_SUCCESS)
    {
        result = task_fshandle_add(task, connection_handle_index, connection_handle);

        if (result == ERR_SUCCESS)
        {
            result = task_fshandle_add(task, connection_handle_index, connection_handle);

            if (result != ERR_SUCCESS)
            {
                fshandle_destroy(connection_handle);
            }
        }
    }

    task_fshandle_release(task, socket_handle_index);

    return result;
}

error_t task_fshandle_payload(Task *task, int handle_index, Message *message)
{
    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return ERR_BAD_FILE_DESCRIPTOR;
    }

    error_t result = fshandle_payload(handle, message);

    task_fshandle_release(task, handle_index);

    return result;
}

error_t task_fshandle_discard(Task *task, int handle_index)
{
    FsHandle *handle = task_fshandle_acquire(task, handle_index);

    if (handle == NULL)
    {
        return ERR_BAD_FILE_DESCRIPTOR;
    }

    error_t result = fshandle_discard(handle);

    task_fshandle_release(task, handle_index);

    return result;
}
