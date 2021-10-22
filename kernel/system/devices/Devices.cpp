#include <libutils/Vec.h>
#include <skift/NumberFormatter.h>
#include <string.h>

#include "system/Streams.h"

#include "pci/PCI.h"
#include "ps2/Legacy.h"
#include "system/devices/Devices.h"
#include "system/devices/Driver.h"
#include "unix/UNIX.h"

static Vec<Ref<Device>> *_devices = nullptr;

static int _device_ids[(uint8_t)DeviceClass::__COUNT] = {};
static const char *_device_names[(uint8_t)DeviceClass::__COUNT] = {
#define DEVICE_NAMES_ENTRY(__type, __name) #__name,
    DEVICE_CLASS_LIST(DEVICE_NAMES_ENTRY)};

void device_scan(IterFunc<DeviceAddress> callback)
{
    if (legacy_scan([&](LegacyAddress address)
            { return callback(DeviceAddress(address)); }) == Iter::STOP)
    {
        return;
    }

    if (pci_scan([&](PCIAddress address)
            { return callback(DeviceAddress(address)); }) == Iter::STOP)
    {
        return;
    }

    if (unix_scan([&](UNIXAddress address)
            { return callback(DeviceAddress(address)); }) == Iter::STOP)
    {
        return;
    }
}

String device_claim_name(DeviceClass klass)
{
    _device_ids[(uint8_t)klass]++;

    if (_device_ids[(uint8_t)klass] == 1)
    {
        return _device_names[(uint8_t)klass];
    }
    else
    {
        return IO::format(
            "{}{}",
            _device_names[(uint8_t)klass],
            _device_ids[(uint8_t)klass] - 1);
    }
}

void device_iterate(IterFunc<Ref<Device>> callback)
{
    if (_devices)
    {
        _devices->foreach([&](auto &device)
            {
                if (callback(device) == Iter::STOP)
                {
                    return Iter::STOP;
                }

                return device->iterate(callback);
            });
    }
}

void devices_acknowledge_interrupt(int interrupt)
{
    device_iterate([&](auto device)
        {
            if (device->interrupt() == interrupt)
            {
                device->acknowledge_interrupt();
            }

            return Iter::CONTINUE;
        });
}

void devices_handle_interrupt(int interrupt)
{
    device_iterate([&](auto device)
        {
            if (device->interrupt() == interrupt)
            {
                device->handle_interrupt();
            }

            return Iter::CONTINUE;
        });
}

void device_initialize()
{
    pci_initialize();

    Kernel::logln("Initializing devices...");

    _devices = new Vec<Ref<Device>>();

    device_scan([&](DeviceAddress address)
        {
            Kernel::logln("Initializing device {}...", address.as_static_cstring());

            auto driver = driver_for(address);

            if (!driver)
            {
                Kernel::logln("No driver found!");

                return Iter::CONTINUE;
            }

            Kernel::logln("Found a driver: {}", driver->name());

            auto device = driver->instance(address);
            if (!device->did_fail())
            {
                _devices->push_back(device);
            }

            return Iter::CONTINUE;
        });
}
