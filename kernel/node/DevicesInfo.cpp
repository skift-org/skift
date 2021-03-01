
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/math/MinMax.h>
#include <libutils/json/Json.h>
#include <string.h>

#include "kernel/devices/Devices.h"
#include "kernel/interrupts/Interupts.h"
#include "kernel/node/DevicesInfo.h"
#include "kernel/node/Handle.h"
#include "kernel/scheduling/Scheduler.h"

FsDeviceInfo::FsDeviceInfo() : FsNode(FILE_TYPE_DEVICE)
{
}

Result FsDeviceInfo::open(FsHandle &handle)
{
    json::Value::Array root{};

    device_iterate([&](RefPtr<Device> device) {
        json::Value::Object device_object{};

        __unused(device);

        device_object["name"] = device->name().cstring();
        device_object["path"] = device->path().cstring();
        device_object["address"] = device->address().as_static_cstring();
        device_object["interrupt"] = device->interrupt();
        device_object["refcount"] = device->refcount();

        auto *driver = driver_for(device->address());
        if (driver)
        {
            device_object["description"] = driver->name();
        }

        root.push_back(device_object);

        return Iteration::CONTINUE;
    });

    Prettifier pretty{};
    json::prettify(pretty, root);

    handle.attached = pretty.finalize().underlying_storage().give_ref();
    handle.attached_size = reinterpret_cast<StringStorage *>(handle.attached)->length();

    return SUCCESS;
}

void FsDeviceInfo::close(FsHandle &handle)
{
    deref_if_not_null(reinterpret_cast<StringStorage *>(handle.attached));
}

ResultOr<size_t> FsDeviceInfo::read(FsHandle &handle, void *buffer, size_t size)
{
    size_t read = 0;

    if (handle.offset() <= handle.attached_size)
    {
        read = MIN(handle.attached_size - handle.offset(), size);
        memcpy(buffer, reinterpret_cast<StringStorage *>(handle.attached)->cstring() + handle.offset(), read);
    }

    return read;
}

void device_info_initialize()
{
    scheduler_running()
        ->domain()
        .link(Path::parse("/System/devices"), make<FsDeviceInfo>());
}
