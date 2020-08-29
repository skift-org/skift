
#include <libjson/Json.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/core/CString.h>
#include <libsystem/math/MinMax.h>
#include <libsystem/thread/Atomic.h>

#include "kernel/devices/Devices.h"
#include "kernel/filesystem/Filesystem.h"
#include "kernel/node/DevicesInfo.h"
#include "kernel/node/Handle.h"

static Iteration append_device_info(json::Value *root, DeviceInfo device)
{
    auto task_object = json::create_object();

    json::object_put(task_object, "device", json::create_string(device_to_static_string(device)));

    const DeviceDriverInfo *driver = device_get_diver_info(device);

    if (driver)
    {
        json::object_put(task_object, "description", json::create_string(driver->description));
    }
    else
    {
        json::object_put(task_object, "description", json::create_string("Unknown"));
    }

    json::array_append(root, task_object);

    return Iteration::CONTINUE;
}

static Result device_info_open(FsDeviceInfo *node, FsHandle *handle)
{
    __unused(node);

    auto root = json::create_array();

    device_iterate(root, (DeviceIterateCallback)append_device_info);

    handle->attached = json::stringify(root);
    handle->attached_size = strlen((const char *)handle->attached);

    json::destroy(root);

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

    if (handle == nullptr)
    {
        return 0;
    }
    else
    {
        return handle->attached_size;
    }
}

FsDeviceInfo::FsDeviceInfo() : FsNode(FILE_TYPE_DEVICE)
{
    open = (FsNodeOpenCallback)device_info_open;
    close = (FsNodeCloseCallback)device_info_close;
    read = (FsNodeReadCallback)device_info_read;
    size = (FsNodeSizeCallback)device_info_size;
}

void device_info_initialize()
{
    auto device_info_device = new FsDeviceInfo();

    Path *device_info_device_path = path_create("/System/devices");
    filesystem_link_and_take_ref(device_info_device_path, device_info_device);
    path_destroy(device_info_device_path);
}
