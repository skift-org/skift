
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

static Result device_info_open(FsDeviceInfo *node, FsHandle *handle)
{
    __unused(node);

    auto root = json::create_array();

    device_iterate([&](RefPtr<Device> device) {
        auto device_object = json::create_object();

        auto *driver = driver_for(device->address());

        __unused(device);

        json::object_put(device_object, "name", json::create_string(device->name().cstring()));
        json::object_put(device_object, "path", json::create_string(device->path().cstring()));
        json::object_put(device_object, "address", json::create_string(device->address().as_static_cstring()));
        json::object_put(device_object, "interrupt", json::create_integer(device->interrupt()));
        json::object_put(device_object, "refcount", json::create_integer(device->refcount()));
        json::object_put(device_object, "description", json::create_string(driver->name()));

        json::array_append(root, device_object);

        return Iteration::CONTINUE;
    });

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
    size = (FsNodeSizeCallback)device_info_size;
}

ResultOr<size_t> FsDeviceInfo::read(FsHandle &handle, void *buffer, size_t size)
{
    size_t read = 0;

    if (handle.offset <= handle.attached_size)
    {
        read = MIN(handle.attached_size - handle.offset, size);
        memcpy(buffer, (char *)handle.attached + handle.offset, read);
    }

    return read;
}

void device_info_initialize()
{
    auto device_info_device = new FsDeviceInfo();

    Path *device_info_device_path = path_create("/System/devices");
    filesystem_link_and_take_ref(device_info_device_path, device_info_device);
    path_destroy(device_info_device_path);
}
