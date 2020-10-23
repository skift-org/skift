#include "kernel/filesystem/Filesystem.h"
#include "kernel/tasking/Task.h"

Path task_resolve_directory_internal(Task *task, const char *buffer)
{
    lock_assert(task->directory_lock);

    Path path{buffer};

    if (path.relative())
    {
        path = *task->directory + path;
    }

    path = path.normalized();

    return path;
}

Path task_resolve_directory(Task *task, const char *buffer)
{
    LockHolder holder(task->directory_lock);

    return task_resolve_directory_internal(task, buffer);
}

Result task_set_directory(Task *task, const char *buffer)
{
    LockHolder holder(task->directory_lock);

    Path path = task_resolve_directory_internal(task, buffer);
    auto node = filesystem_find(path);

    if (node == nullptr)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (node->type() != FILE_TYPE_DIRECTORY)
    {
        return ERR_NOT_A_DIRECTORY;
    }

    *task->directory = path;

    return SUCCESS;
}

Result task_get_directory(Task *task, char *buffer, uint size)
{
    LockHolder holder(task->directory_lock);

    strlcpy(buffer, task->directory->string().cstring(), size);

    return SUCCESS;
}
