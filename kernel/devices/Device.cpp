#include "kernel/devices/Device.h"
#include "kernel/devices/Devices.h"
#include "kernel/node/Device.h"

Device::Device(DeviceAddress address, DeviceClass klass)
    : _address(address),
      _klass(klass),
      _name(device_claim_name(klass))
{
}

OwnPtr<FsHandle> Device::open(OpenFlag flags)
{
    auto fsnode = make<FsDevice>(*this);
    return own<FsHandle>(fsnode, flags);
}
