#include "kernel/filesystem/Filesystem.h"
#include "kernel/tasking/Task.h"

Path *task_resolve_directory(Task *task, const char *buffer)
{
    Path *path = path_create(buffer);

    if (path_is_relative(path))
    {
        lock_acquire(task->directory_lock);

        Path *combined = path_combine(task->directory, path);
        path_destroy(path);
        path = combined;

        lock_release(task->directory_lock);
    }

    path_normalize(path);

    return path;
}

Result task_set_directory(Task *task, const char *buffer)
{
    Result result = SUCCESS;

    Path *path = task_resolve_directory(task, buffer);
    FsNode *node = filesystem_find_and_ref(path);

    if (node == NULL)
    {
        result = ERR_NO_SUCH_FILE_OR_DIRECTORY;
        goto cleanup_and_return;
    }

    if (node->type != FILE_TYPE_DIRECTORY)
    {
        result = ERR_NOT_A_DIRECTORY;
        goto cleanup_and_return;
    }

    lock_acquire(task->directory_lock);

    path_destroy(task->directory);
    task->directory = path;
    path = NULL;

    lock_release(task->directory_lock);

cleanup_and_return:
    if (node)
        fsnode_deref(node);

    if (path)
        path_destroy(path);

    return result;
}

Result task_get_directory(Task *task, char *buffer, uint size)
{
    lock_acquire(task->directory_lock);

    path_to_cstring(task->directory, buffer, size);

    lock_release(task->directory_lock);

    return SUCCESS;
}