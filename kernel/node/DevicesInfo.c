/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libjson/Json.h>
#include <libmath/MinMax.h>
#include <libsystem/Result.h>
#include <libsystem/atomic.h>
#include <libsystem/cstring.h>
#include <libsystem/logger.h>

#include "kernel/devices/Devices.h"
#include "kernel/filesystem/Filesystem.h"
#include "kernel/node/DevicesInfo.h"
#include "kernel/node/Handle.h"

static IterationDecision append_device_info(JsonValue *root, DeviceInfo device)
{
    JsonValue *task_object = json_create_object();

    json_object_put(task_object, "device", json_create_string(device_to_string(device)));

    const DeviceDriverInfo *driver = device_get_diver_info(device);

    if (driver)
    {
        json_object_put(task_object, "description", json_create_string(driver->description));
    }
    else
    {
        json_object_put(task_object, "description", json_create_string("Unknown"));
    }

    json_array_append(root, task_object);

    return ITERATION_CONTINUE;
}

Result device_info_FsOperationOpen(FsDeviceInfo *node, FsHandle *handle)
{
    __unused(node);

    JsonValue *root = json_create_array();

    device_iterate(root, (DeviceIterateCallback)append_device_info);

    handle->attached = json_stringify(root);
    handle->attached_size = strlen((const char *)handle->attached);

    json_destroy(root);

    return SUCCESS;
}

void device_info_FsOperationClose(FsDeviceInfo *node, FsHandle *handle)
{
    __unused(node);

    if (handle->attached)
    {
        free(handle->attached);
    }
}

Result device_info_FsOperationRead(FsDeviceInfo *node, FsHandle *handle, void *buffer, size_t size, size_t *read)
{
    __unused(node);

    if (handle->offset <= handle->attached_size)
    {
        *read = MIN(handle->attached_size - handle->offset, size);
        memcpy(buffer, (byte *)handle->attached + handle->offset, *read);
    }

    return SUCCESS;
}

size_t device_info_FsOperationSize(FsDeviceInfo *node, FsHandle *handle)
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

FsNode *device_info_create(void)
{
    FsDeviceInfo *info = __create(FsDeviceInfo);

    fsnode_init(FSNODE(info), FILE_TYPE_DEVICE);

    FSNODE(info)->open = (FsOperationOpen)device_info_FsOperationOpen;
    FSNODE(info)->read = (FsOperationRead)device_info_FsOperationRead;
    FSNODE(info)->size = (FsOperationSize)device_info_FsOperationSize;

    return (FsNode *)info;
}

void device_info_initialize(void)
{
    FsNode *device_info_device = device_info_create();

    Path *device_info_device_path = path_create("/sys/devices");
    filesystem_link_and_take_ref(device_info_device_path, device_info_device);
    path_destroy(device_info_device_path);
}