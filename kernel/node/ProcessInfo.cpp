
#include <libjson/Json.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/core/CString.h>
#include <libsystem/math/MinMax.h>
#include <libsystem/thread/Atomic.h>

#include "kernel/filesystem/Filesystem.h"
#include "kernel/node/Handle.h"
#include "kernel/node/ProcessInfo.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/tasking/Task-Memory.h"

FsProcessInfo::FsProcessInfo() : FsNode(FILE_TYPE_DEVICE)
{
}

static Iteration serialize_task(json::Value *destination, Task *task)
{
    if (task->id == 0)
        return Iteration::CONTINUE;

    auto task_object = json::create_object();

    json::object_put(task_object, "id", json::create_integer(task->id));
    json::object_put(task_object, "name", json::create_string(task->name));
    json::object_put(task_object, "state", json::create_string(task_state_string(task->state())));
    json::object_put(task_object, "directory", json::create_string_adopt(path_as_string(task->directory)));
    json::object_put(task_object, "cpu", json::create_integer(scheduler_get_usage(task->id)));
    json::object_put(task_object, "ram", json::create_integer(task_memory_usage(task)));
    json::object_put(task_object, "user", json::create_boolean(task->user));

    json::array_append(destination, task_object);

    return Iteration::CONTINUE;
}

Result FsProcessInfo::open(FsHandle *handle)
{
    auto destination = json::create_array();

    task_iterate(destination, (TaskIterateCallback)serialize_task);

    handle->attached = json::stringify(destination);
    handle->attached_size = strlen((const char *)handle->attached);

    json::destroy(destination);

    return SUCCESS;
}

void FsProcessInfo::close(FsHandle *handle)
{
    if (handle->attached)
    {
        free(handle->attached);
    }
}

ResultOr<size_t> FsProcessInfo::read(FsHandle &handle, void *buffer, size_t size)
{
    size_t read = 0;

    if (handle.offset <= handle.attached_size)
    {
        read = MIN(handle.attached_size - handle.offset, size);
        memcpy(buffer, (char *)handle.attached + handle.offset, read);
    }

    return read;
}

void process_info_initialize()
{
    auto info_device = new FsProcessInfo();
    filesystem_link_and_take_ref_cstring("/System/processes", info_device);
}
