#include <abi/Paths.h>
#include <libsystem/Logger.h>

#include "kernel/devices/Device.h"
#include "kernel/devices/Devices.h"
#include "kernel/filesystem/DevicesFileSystem.h"
#include "kernel/node/Device.h"
#include "kernel/scheduling/Scheduler.h"

void devices_filesystem_initialize()
{
    auto &domain = scheduler_running()->domain();
    domain.mkdir(IO::Path::parse(DEVICE_PATH));

    device_iterate([&](auto device) {
        String path = device->path();
        logger_info("Mounting %s to %s", device->address().as_static_cstring(), path.cstring());
        domain.link(IO::Path::parse(path), make<FsDevice>(device));

        return Iteration::CONTINUE;
    });
}
