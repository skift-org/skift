#pragma once

#include "kernel/devices/Device.h"

class Partition : public Device
{
private:
    RefPtr<Device> _disk;
    int _index;
    size64_t _start;
    size64_t _size;

    size64_t end() { return _start + _size; }

public:
    Partition(RefPtr<Device> disk, int index, size64_t start, size64_t size)
        : Device({}, DeviceClass::PARTITION),
          _disk{disk},
          _index{index},
          _start{start},
          _size{size}
    {
    }

    ~Partition()
    {
    }

    bool can_read() override
    {
        return _disk->can_read();
    }

    bool can_write() override
    {
        return _disk->can_write();
    }

    size_t size() override { return _size; }

    ResultOr<size_t> read(size64_t offset, void *buffer, size_t size) override
    {
        size64_t final_offset = _start + offset;
        size64_t remaining = end() - final_offset;

        return _disk->read(final_offset, buffer, MIN(remaining, size));
    }

    ResultOr<size_t> write(size64_t offset, const void *buffer, size_t size) override
    {
        size64_t final_offset = _start + offset;
        size64_t remaining = end() - final_offset;

        return _disk->write(final_offset, buffer, MIN(remaining, size));
    }
};