#pragma once

#include "kernel/devices/Device.h"
#include "kernel/devices/DeviceAddress.h"

class DeviceMatcher
{
private:
    DeviceBus _bus;
    const char *_name;

public:
    DeviceBus bus() const { return _bus; }

    const char *name() const { return _name; }

    constexpr DeviceMatcher(DeviceBus bus, const char *name)
        : _bus(bus),
          _name(name)
    {
    }

    virtual ~DeviceMatcher() {}

    virtual bool match(DeviceAddress address) = 0;

    virtual RefPtr<Device> instance(DeviceAddress address) = 0;
};

void driver_initialize();

DeviceMatcher *driver_for(DeviceAddress address);
