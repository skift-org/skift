#include "kernel/storage/Partitions.h"
#include "kernel/devices/Devices.h"
#include "kernel/storage/MBR.h"

bool partition_load_mbr(FsHandle &handle, Device &device, const MBR &mbr)
{
    __unused(handle);
    __unused(device);

    if (mbr.magic != 0xAA55)
    {
        return false;
    }

    logger_info("MBR on '%s': ", device.path().cstring());
    logger_info("    - magic     = 0x%04x", mbr.magic);
    logger_info("    - signature = 0x%08x", mbr.signature);

    for (size_t i = 0; i < 4; i++)
    {
        const MBREntry &entry = mbr.entries[i];

        logger_info("    - Partition[%d] = {start=%8d, size=%8d, type=0x%01x}", i, entry.start, entry.size, entry.type);
    }

    return true;
}

bool partition_load_gpt(FsHandle &handle, Device &device, const MBR &mbr)
{
    __unused(handle);
    __unused(device);
    __unused(mbr);

    // TODO: GPT partition support

    return false;
}

void partitions_initialize()
{
    device_iterate([](RefPtr<Device> device) {
        if (device->klass() == DeviceClass::DISK)
        {
            MBR mbr;
            auto handle = device->open(OPEN_READ);
            handle->read(&mbr, sizeof(MBR));

            bool r = partition_load_mbr(*handle, *device, mbr) ||
                     partition_load_gpt(*handle, *device, mbr);

            if (!r)
            {
                logger_warn("Device '%s' don't have a partion table!", device->path().cstring());
            }
        }

        return Iteration::CONTINUE;
    });
}