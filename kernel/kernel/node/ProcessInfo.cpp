#include <string.h>

#include <libjson/Json.h>
#include <libmath/MinMax.h>
#include <libsystem/Result.h>

#include "kernel/interrupts/Interupts.h"
#include "kernel/node/Handle.h"
#include "kernel/node/ProcessInfo.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/tasking/Task-Memory.h"

FsProcessInfo::FsProcessInfo() : FsNode(FILE_TYPE_DEVICE)
{
}

static Iteration serialize_task(Json::Value::Array *list, Task *task)
{
    if (task->id == 0)
    {
        return Iteration::CONTINUE;
    }

    Json::Value::Object task_object{};

    task_object["id"] = (int64_t)task->id;
    task_object["name"] = task->name;
    task_object["state"] = task_state_string(task->state());
    task_object["cpu"] = (int64_t)scheduler_get_usage(task->id);
    task_object["ram"] = (int64_t)task_memory_usage(task);
    task_object["user"] = (task->_flags & TASK_USER) == TASK_USER;

    list->push_back(move(task_object));

    return Iteration::CONTINUE;
}

Result FsProcessInfo::open(FsHandle &handle)
{
    Json::Value::Array list{};

    task_iterate(&list, (TaskIterateCallback)serialize_task);

    Prettifier pretty{};
    Json::prettify(pretty, list);

    handle.attached = pretty.finalize().storage().give_ref();
    handle.attached_size = reinterpret_cast<StringStorage *>(handle.attached)->size();

    return SUCCESS;
}

void FsProcessInfo::close(FsHandle &handle)
{
    deref_if_not_null(reinterpret_cast<StringStorage *>(handle.attached));
}

ResultOr<size_t> FsProcessInfo::read(FsHandle &handle, void *buffer, size_t size)
{
    size_t read = 0;

    if (handle.offset() <= handle.attached_size)
    {
        read = MIN(handle.attached_size - handle.offset(), size);
        memcpy(buffer, reinterpret_cast<StringStorage *>(handle.attached)->cstring() + handle.offset(), read);
    }

    return read;
}

void process_info_initialize()
{
    scheduler_running()->domain().link(Path::parse("/System/processes"), make<FsProcessInfo>());
}
