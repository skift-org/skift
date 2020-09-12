#pragma once

#include "kernel/bus/UNIX.h"

class UNIXDevice : public Device
{
private:
public:
    UNIXDevice(DeviceAddress address, DeviceClass klass)
        : Device(address, klass)
    {
    }

    ~UNIXDevice()
    {
    }
};

template <typename UNIXDeviceType>
class UNIXDeviceDriver : public DeviceDriver
{
private:
    UNIXAddress _address;

public:
    UNIXDeviceDriver(const char *name, UNIXAddress address)
        : DeviceDriver(BUS_UNIX, name),
          _address(address)
    {
    }

    bool match(DeviceAddress address)
    {
        return _address == address.unix();
    }

    RefPtr<Device> instance(DeviceAddress address)
    {
        return make<UNIXDeviceType>(address);
    }
};
