#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/utils/NumberFormatter.h>
#include <libutils/StringBuilder.h>
#include <libutils/Vector.h>

#include "kernel/bus/Legacy.h"
#include "kernel/bus/PCI.h"
#include "kernel/bus/UNIX.h"
#include "kernel/devices/Devices.h"
#include "kernel/devices/Driver.h"

static Vector<RefPtr<Device>> *_devices = nullptr;

static int _device_ids[(uint8_t)DeviceClass::__COUNT] = {};
static const char *_device_names[(uint8_t)DeviceClass::__COUNT] = {
#define DEVICE_NAMES_ENTRY(__type, __name) #__name,
    DEVICE_CLASS_LIST(DEVICE_NAMES_ENTRY)};

void device_scan(IterationCallback<DeviceAddress> callback)
{
    if (legacy_scan([&](LegacyAddress address) {
            return callback(DeviceAddress(address));
        }) == Iteration::STOP)
    {
        return;
    }

    if (pci_scan([&](PCIAddress address) {
            return callback(DeviceAddress(address));
        }) == Iteration::STOP)
    {
        return;
    }

    if (unix_scan([&](UNIXAddress address) {
            return callback(DeviceAddress(address));
        }) == Iteration::STOP)
    {
        return;
    }
}

String device_claim_name(DeviceClass klass)
{
    const char *name = _device_names[(uint8_t)klass];

    StringBuilder builder{};

    builder.append(name);

    if (_device_ids[(uint8_t)klass] > 0)
    {
        char number_buffer[12] = {};
        format_int(FORMAT_DECIMAL, _device_ids[(uint8_t)klass], number_buffer, 12);
        builder.append(number_buffer);
    }

    _device_ids[(uint8_t)klass]++;

    return builder.finalize();
}

void device_iterate(IterationCallback<RefPtr<Device>> callback)
{
    if (_devices)
    {
        _devices->foreach ([&](auto &device) {
            if (callback(device) == Iteration::STOP)
            {
                return Iteration::STOP;
            }

            return device->iterate(callback);
        });
    }
}

void devices_acknowledge_interrupt(int interrupt)
{
    device_iterate([&](auto device) {
        if (device->interrupt() == interrupt)
        {
            device->acknowledge_interrupt();
        }

        return Iteration::CONTINUE;
    });
}

void devices_handle_interrupt(int interrupt)
{
    device_iterate([&](auto device) {
        if (device->interrupt() == interrupt)
        {
            device->handle_interrupt();
        }

        return Iteration::CONTINUE;
    });
}

void device_initialize()
{
    pci_initialize();

    logger_info("Initializing devices...");

    _devices = new Vector<RefPtr<Device>>();

    device_scan([&](DeviceAddress address) {
        logger_info("Initializing device %s...", address.as_static_cstring());

        auto driver = driver_for(address);

        if (!driver)
        {
            logger_warn("No driver found!");

            return Iteration::CONTINUE;
        }

        logger_info("Found a driver: %s", driver->name());

        auto device = driver->instance(address);
        if (!device->did_fail())
        {
            _devices->push_back(device);
        }

        return Iteration::CONTINUE;
    });
}
