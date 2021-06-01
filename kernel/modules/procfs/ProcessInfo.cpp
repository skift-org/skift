#include <abi/Result.h>
#include <string.h>

#include <libjson/Json.h>
#include <libmath/MinMax.h>

#include "procfs/ProcessInfo.h"
#include "system/interrupts/Interupts.h"
#include "system/node/Handle.h"
#include "system/scheduling/Scheduler.h"
#include "system/tasking/Task-Memory.h"

FsProcessInfo::FsProcessInfo() : FsNode(HJ_FILE_TYPE_DEVICE)
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

    list->push_back(std::move(task_object));

    return Iteration::CONTINUE;
}

HjResult FsProcessInfo::open(FsHandle &handle)
{
    Json::Value::Array list{};

    task_iterate(&list, (TaskIterateCallback)serialize_task);

    auto str = Json::stringify(list);
    handle.attached = str.storage().give_ref();
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
    scheduler_running()->domain().link(IO::Path::parse("/System/processes"), make<FsProcessInfo>());
}
