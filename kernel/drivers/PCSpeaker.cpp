#include "kernel/drivers/PCSpeaker.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/tasking/Task.h"

PCSpeaker::PCSpeaker(DeviceAddress address) : LegacyDevice(address, DeviceClass::PCSPEAKER) {}

PCSpeaker::~PCSpeaker() {}

void PCSpeaker::note(int length, int freq)
{

    uint8_t t;

    if (length == 0)
    {
        t = in8(0x61) & 0xFC;
        out8(0x61, t);
        return;
    }

    uint32_t div = 11931800 / freq;

    out8(0x43, 0xb6);
    out8(0x42, (uint8_t)(div));
    out8(0x42, (uint8_t)(div >> 8));

    t = in8(0x61);
    out8(0x61, t | 0x3);

    if (length > 0)
    {

        task_sleep(scheduler_running(), length);

        t = in8(0x61) & 0xFC;
        out8(0x61, t);
    }
}

ResultOr<size_t> PCSpeaker::write(size64_t offset, const void *buffer, size_t size)
{
    __unused(offset);
    if (!size % (sizeof(struct Speaker)))
    {
        return 0;
    }

    struct Speaker *s = (struct Speaker *)buffer;
    while ((uintptr_t)s < (uintptr_t)buffer + size)
    {
        note(s->length, s->frequency);
        s++;
    }

    return (uintptr_t)s - (uintptr_t)buffer;
}
