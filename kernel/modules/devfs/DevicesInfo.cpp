#include <abi/Result.h>
#include <libjson/Json.h>
#include <libmath/MinMax.h>
#include <string.h>

#include "devfs/DevicesInfo.h"
#include "system/devices/Devices.h"
#include "system/interrupts/Interupts.h"
#include "system/node/Handle.h"
#include "system/scheduling/Scheduler.h"

FsDeviceInfo::FsDeviceInfo() : FsNode(HJ_FILE_TYPE_DEVICE)
{
}

HjResult FsDeviceInfo::open(FsHandle &handle)
{
    Json::Value::Array root{};

    device_iterate([&](RefPtr<Device> device) {
        Json::Value::Object device_object{};

        UNUSED(device);

        device_object["name"] = device->name().cstring();
        device_object["path"] = device->path().cstring();
        device_object["address"] = device->address().as_static_cstring();
        device_object["interrupt"] = (int64_t)device->interrupt();
        device_object["refcount"] = (int64_t)device->refcount();

        auto *driver = driver_for(device->address());
        if (driver)
        {
            device_object["description"] = driver->name();
        }

        root.push_back(device_object);

        return Iteration::CONTINUE;
    });

    auto str = Json::stringify(root);
    handle.attached = str.storage().give_ref();
    handle.attached_size = reinterpret_cast<StringStorage *>(handle.attached)->size();

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
        .link(IO::Path::parse("/System/devices"), make<FsDeviceInfo>());
}
