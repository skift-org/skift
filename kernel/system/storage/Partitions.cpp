#include "system/Streams.h"

#include "mbr/MBR.h"
#include "system/devices/Devices.h"
#include "system/storage/Partition.h"
#include "system/storage/Partitions.h"

bool partition_load_mbr(RefPtr<Device> disk, const MBR &mbr)
{
    if (mbr.magic != 0xAA55)
    {
        return false;
    }

    Kernel::logln("MBR on '{}': ", disk->path().cstring());
    Kernel::logln("    - magic     = {#04x}", mbr.magic);
    Kernel::logln("    - signature = {#08x}", mbr.signature);

    for (size_t i = 0; i < 4; i++)
    {
        const MBREntry &entry = mbr.entries[i];

        Kernel::logln("    - Partition[{}] = {start={8d}, size={8d}, type=0x{x}}", i, entry.start, entry.size, entry.type);

        if (entry.type != 0)
        {
            disk->add(make<Partition>(disk, i, entry.start * 512, entry.size * 512));
        }
    }

    return true;
}

bool partition_load_gpt(RefPtr<Device> disk, const MBR &mbr)
{
    UNUSED(disk);
    UNUSED(mbr);

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
                Kernel::logln("Device '{}' don't have a valid partion table!", device->path());
            }
        }

        return Iteration::CONTINUE;
    });
}