#include "kernel/devices/Device.h"
#include "kernel/devices/Devices.h"

Device::Device(DeviceAddress address, DeviceClass klass)
    : _address(address),
      _klass(klass),
      _name(device_claim_name(klass))
{
}
