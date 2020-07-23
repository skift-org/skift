
#include <libjson/Json.h>
#include <libsystem/Atomic.h>
#include <libsystem/CString.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/math/MinMax.h>

#include "kernel/devices/Devices.h"
#include "kernel/filesystem/Filesystem.h"
#include "kernel/node/DevicesInfo.h"
#include "kernel/node/Handle.h"

static IterationDecision append_device_info(JsonValue *root, DeviceInfo device)
{
    JsonValue *task_object = json_create_object();

    json_object_put(task_object, "device", json_create_string(device_to_static_string(device)));

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

static Result device_info_open(FsDeviceInfo *node, FsHandle *handle)
{
    __unused(node);

    JsonValue *root = json_create_array();

    device_iterate(root, (DeviceIterateCallback)append_device_info);

    handle->attached = json_stringify(root);
    handle->attached_size = strlen((const char *)handle->attached);

    json_destroy(root);

    return SUCCESS;
}

static void device_info_close(FsDeviceInfo *node, FsHandle *handle)
{
    __unused(node);

    if (handle->attached)
    {
        free(handle->attached);
    }
}

static Result device_info_read(FsDeviceInfo *node, FsHandle *handle, void *buffer, size_t size, size_t *read)
{
    __unused(node);

    if (handle->offset <= handle->attached_size)
    {
        *read = MIN(handle->attached_size - handle->offset, size);
        memcpy(buffer, (char *)handle->attached + handle->offset, *read);
    }

    return SUCCESS;
}

static size_t device_info_size(FsDeviceInfo *node, FsHandle *handle)
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

static FsNode *device_info_create(void)
{
    FsDeviceInfo *info = __create(FsDeviceInfo);

    fsnode_init(FSNODE(info), FILE_TYPE_DEVICE);

    FSNODE(info)->open = (FsNodeOpenCallback)device_info_open;
    FSNODE(info)->close = (FsNodeCloseCallback)device_info_close;
    FSNODE(info)->read = (FsNodeReadCallback)device_info_read;
    FSNODE(info)->size = (FsNodeSizeCallback)device_info_size;

    return (FsNode *)info;
}

void device_info_initialize(void)
{
    FsNode *device_info_device = device_info_create();

    Path *device_info_device_path = path_create("/System/devices");
    filesystem_link_and_take_ref(device_info_device_path, device_info_device);
    path_destroy(device_info_device_path);
}
