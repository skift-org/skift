#include <abi/Paths.h>
#include <libsystem/Logger.h>

#include "kernel/devices/Device.h"
#include "kernel/devices/Devices.h"
#include "kernel/filesystem/DevicesFileSystem.h"
#include "kernel/filesystem/Filesystem.h"
#include "kernel/node/Device.h"

void devices_filesystem_initialize()
{
    filesystem_mkdir(Path::parse(DEVICE_PATH));

    device_iterate([](auto device) {
        String path = device->path();
        logger_info("Mounting %s to %s", device->address().as_static_cstring(), path.cstring());
        filesystem_link(Path::parse(path), make<FsDevice>(device));

        return Iteration::CONTINUE;
    });
}
