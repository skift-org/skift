#pragma once

#include "pcspkr/PCSpeaker.h"
#include "ps2/LegacyDevice.h"
#include "system/devices/Device.h"
#include "system/scheduling/Scheduler.h"
#include "system/tasking/Task.h"

struct Speaker
{
    int length;
    int frequency;
};

class PCSpeaker : public LegacyDevice
{
private:
    void note(int length, int freq);

public:
    PCSpeaker(DeviceAddress address);

    ~PCSpeaker();

    ResultOr<size_t> write(size64_t offset, const void *buffer, size_t size) override;
};
