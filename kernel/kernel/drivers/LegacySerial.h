#pragma once

#include <libutils/RingBuffer.h>
#include <skift/Lock.h>

#include "archs/x86/kernel/COM.h"

#include "kernel/devices/LegacyDevice.h"

class LegacySerial : public LegacyDevice
{
private:
    RingBuffer<char> _buffer{4096};
    Lock _buffer_lock{"legacy-serial"};

    COMPort port() { return (COMPort)legacy_address(); }

public:
    LegacySerial(DeviceAddress address);

    void handle_interrupt() override;

    bool can_read() override;

    ResultOr<size_t> read(size64_t offset, void *buffer, size_t size) override;

    ResultOr<size_t> write(size64_t offset, const void *buffer, size_t size) override;
};
