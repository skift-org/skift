#include "kernel/filesystem/Filesystem.h"
#include "kernel/tasking/Task.h"

Path task_resolve_directory_internal(Task *task, Path &path)
{
    lock_assert(task->directory_lock);

    if (path.relative())
    {
        path = Path::join(*task->directory, path);
    }

    path = path.normalized();

    return path;
}

Path task_resolve_directory(Task *task, Path &path)
{
    LockHolder holder(task->directory_lock);

    return task_resolve_directory_internal(task, path);
}

Result task_set_directory(Task *task, Path &path)
{
    LockHolder holder(task->directory_lock);

    auto resolved_path = task_resolve_directory_internal(task, path);
    auto node = filesystem_find(path);

    if (node == nullptr)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (node->type() != FILE_TYPE_DIRECTORY)
    {
        return ERR_NOT_A_DIRECTORY;
    }

    *task->directory = resolved_path;

    return SUCCESS;
}

Path task_get_directory(Task *task)
{
    LockHolder holder(task->directory_lock);
    return *task->directory;
}
