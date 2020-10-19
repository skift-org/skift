#include "kernel/filesystem/Filesystem.h"
#include "kernel/tasking/Task.h"

Path *task_resolve_directory_internal(Task *task, const char *buffer)
{
    lock_assert(task->directory_lock);

    Path *path = path_create(buffer);

    if (path_is_relative(path))
    {
        Path *combined = path_combine(task->directory, path);
        path_destroy(path);
        path = combined;
    }

    path_normalize(path);

    return path;
}

Path *task_resolve_directory(Task *task, const char *buffer)
{
    LockHolder holder(task->directory_lock);

    return task_resolve_directory_internal(task, buffer);
}

Result task_set_directory(Task *task, const char *buffer)
{
    LockHolder holder(task->directory_lock);
    Result result = SUCCESS;

    Path *path = task_resolve_directory_internal(task, buffer);
    auto node = filesystem_find_and_ref(path);

    if (node == nullptr)
    {
        result = ERR_NO_SUCH_FILE_OR_DIRECTORY;
        goto cleanup_and_return;
    }

    if (node->type() != FILE_TYPE_DIRECTORY)
    {
        result = ERR_NOT_A_DIRECTORY;
        goto cleanup_and_return;
    }

    path_destroy(task->directory);
    task->directory = path;
    path = nullptr;

cleanup_and_return:
    if (path)
        path_destroy(path);

    return result;
}

Result task_get_directory(Task *task, char *buffer, uint size)
{
    LockHolder holder(task->directory_lock);

    path_to_cstring(task->directory, buffer, size);

    return SUCCESS;
}
