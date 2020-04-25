/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libjson/Json.h>
#include <libmath/MinMax.h>
#include <libsystem/Result.h>
#include <libsystem/atomic.h>
#include <libsystem/cstring.h>
#include <libsystem/logger.h>

#include "kernel/node/Handle.h"
#include "kernel/node/ProcessInfo.h"
#include "kernel/tasking.h"

Result info_FsOperationOpen(FsInfo *node, FsHandle *handle)
{
    __unused(node);

    JsonValue *root = json_create_array();

    atomic_begin();

    list_foreach(Task, task, task_all())
    {
        JsonValue *task_object = json_create_object();

        json_object_put(task_object, "id", json_create_integer(task->id));
        json_object_put(task_object, "name", json_create_string(task->name));
        json_object_put(task_object, "cwd", json_create_string_adopt(path_as_string(task->cwd_path)));
        json_object_put(task_object, "cpu", json_create_integer(sheduler_get_usage(task->id)));

        json_array_append(root, task_object);
    }

    atomic_end();

    handle->attached = json_stringify(root);
    handle->attached_size = strlen((const char *)handle->attached);

    json_destroy(root);

    return SUCCESS;
}

void info_FsOperationClose(FsInfo *node, FsHandle *handle)
{
    __unused(node);

    if (handle->attached)
    {
        free(handle->attached);
    }
}

Result info_FsOperationRead(FsInfo *node, FsHandle *handle, void *buffer, size_t size, size_t *readed)
{
    __unused(node);

    if (handle->offset <= handle->attached_size)
    {
        *readed = MIN(handle->attached_size - handle->offset, size);
        memcpy(buffer, (byte *)handle->attached + handle->offset, *readed);
    }

    return SUCCESS;
}

size_t info_FsOperationSize(FsInfo *node, FsHandle *handle)
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

FsNode *info_create(void)
{
    FsInfo *info = __create(FsInfo);

    fsnode_init(FSNODE(info), FILE_TYPE_DEVICE);

    FSNODE(info)->open = (FsOperationOpen)info_FsOperationOpen;
    FSNODE(info)->read = (FsOperationRead)info_FsOperationRead;
    FSNODE(info)->size = (FsOperationSize)info_FsOperationSize;

    return (FsNode *)info;
}

void info_initialize(void)
{
    FsNode *info_device = info_create();

    Path *info_device_path = path_create("/sys/processes");
    filesystem_link_and_take_ref(info_device_path, info_device);
    path_destroy(info_device_path);
}