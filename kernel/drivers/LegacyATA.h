#pragma once

#include <libsystem/thread/Lock.h>

#include "kernel/devices/LegacyDevice.h"

class LegacyATA : public LegacyDevice
{
private:
    Lock _buffer_lock{"legacy-ata"};

    void identify();
    void select();
    void has_failed(uint8_t status);

    int _bus;
    int _drive;
    uint16_t _ide_buffer[256];
    bool _exists = false;

public:
    LegacyATA(DeviceAddress address);

    bool can_read(FsHandle &handle) override;

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size) override;

    ResultOr<size_t> write(FsHandle &handle, const void *buffer, size_t size) override;
};
