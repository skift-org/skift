#include "kernel/storage/Partitions.h"
#include "kernel/devices/Devices.h"
#include "kernel/storage/MBR.h"
#include "kernel/storage/Partition.h"

bool partition_load_mbr(RefPtr<Device> disk, const MBR &mbr)
{
    if (mbr.magic != 0xAA55)
    {
        return false;
    }

    logger_info("MBR on '%s': ", disk->path().cstring());
    logger_info("    - magic     = 0x%04x", mbr.magic);
    logger_info("    - signature = 0x%08x", mbr.signature);

    for (size_t i = 0; i < 4; i++)
    {
        const MBREntry &entry = mbr.entries[i];

        logger_info("    - Partition[%d] = {start=%8d, size=%8d, type=0x%01x}", i, entry.start, entry.size, entry.type);

        if (entry.type != 0)
        {
            disk->add(make<Partition>(disk, i, entry.start * 512, entry.size * 512));
        }
    }

    return true;
}

bool partition_load_gpt(RefPtr<Device> disk, const MBR &mbr)
{
    __unused(disk);
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
            device->read(0, &mbr, sizeof(MBR));

            bool success = partition_load_gpt(device, mbr) ||
                           partition_load_mbr(device, mbr);

            if (!success)
            {
                logger_warn("Device '%s' don't have a valid partion table!", device->path().cstring());
            }
        }

        return Iteration::CONTINUE;
    });
}