
#include <libjson/Json.h>
#include <libmath/MinMax.h>
#include <libsystem/Atomic.h>
#include <libsystem/CString.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>

#include "kernel/node/Handle.h"
#include "kernel/node/ProcessInfo.h"
#include "kernel/tasking.h"

static Result process_info_open(FsProcessInfo *node, FsHandle *handle)
{
    __unused(node);

    JsonValue *root = json_create_array();

    atomic_begin();

    list_foreach(Task, task, task_all())
    {
        JsonValue *task_object = json_create_object();

        json_object_put(task_object, "id", json_create_integer(task->id));
        json_object_put(task_object, "name", json_create_string(task->name));
        json_object_put(task_object, "state", json_create_string(task_state_string(task->state)));
        json_object_put(task_object, "cwd", json_create_string_adopt(path_as_string(task->cwd_path)));
        json_object_put(task_object, "cpu", json_create_integer(scheduler_get_usage(task->id)));
        json_object_put(task_object, "user", json_create_boolean(task->user));

        json_array_append(root, task_object);
    }

    atomic_end();

    handle->attached = json_stringify(root);
    handle->attached_size = strlen((const char *)handle->attached);

    json_destroy(root);

    return SUCCESS;
}

static void process_info_close(FsProcessInfo *node, FsHandle *handle)
{
    __unused(node);

    if (handle->attached)
    {
        free(handle->attached);
    }
}

static Result process_info_read(FsProcessInfo *node, FsHandle *handle, void *buffer, size_t size, size_t *read)
{
    __unused(node);

    if (handle->offset <= handle->attached_size)
    {
        *read = MIN(handle->attached_size - handle->offset, size);
        memcpy(buffer, (byte *)handle->attached + handle->offset, *read);
    }

    return SUCCESS;
}

static size_t process_info_size(FsProcessInfo *node, FsHandle *handle)
{
    __unused(node);

    if (handle == NULL)
    {
        return 0;
    }
    else
    {
        return handle->attached_size;
    }
}

static FsNode *process_info_create(void)
{
    FsProcessInfo *info = __create(FsProcessInfo);

    fsnode_init(FSNODE(info), FILE_TYPE_DEVICE);

    FSNODE(info)->open = (FsNodeOpenCallback)process_info_open;
    FSNODE(info)->close = (FsNodeCloseCallback)process_info_close;
    FSNODE(info)->read = (FsNodeReadCallback)process_info_read;
    FSNODE(info)->size = (FsNodeSizeCallback)process_info_size;

    return (FsNode *)info;
}

void process_info_initialize(void)
{
    FsNode *info_device = process_info_create();

    Path *info_device_path = path_create("/sys/processes");
    filesystem_link_and_take_ref(info_device_path, info_device);
    path_destroy(info_device_path);
}
