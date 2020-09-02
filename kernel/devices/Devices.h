#pragma once

#include <libsystem/Common.h>

#include "kernel/bus/PCIDevice.h"

enum Bus
{
    BUS_NONE,
    BUS_UNIX,
    BUS_PCI,
};

enum UNIXDevice
{
    UNIX_ZERO,
    UNIX_NULL,
    UNIX_RANDOM,
};

struct UNIXDeviceAddress
{
    UNIXDevice device;
};

struct DeviceInfo
{
    Bus bus;

    union {
        UNIXDeviceAddress unix_device;
        PCIDevice pci_device;
    };
};

typedef bool (*DeviceDriverInfoMatch)(DeviceInfo info);
typedef void (*DeviceDriverInfoInitialize)(DeviceInfo info);

struct DeviceDriverInfo
{
    const char *description;

    Bus bus;
    DeviceDriverInfoMatch match;
    DeviceDriverInfoInitialize initialize;
};

typedef Iteration (*DeviceIterateCallback)(void *target, DeviceInfo device);

void device_initialize();

const DeviceDriverInfo *device_get_diver_info(DeviceInfo info);

const char *device_to_static_string(DeviceInfo info);

void device_iterate(void *target, DeviceIterateCallback callback);

/* --- Devices -------------------------------------------------------------- */

bool bga_match(DeviceInfo info);
void bga_initialize(DeviceInfo info);

bool e1000_match(DeviceInfo info);
void e1000_initialize(DeviceInfo info);

void keyboard_initialize();

void null_initialize();

void proc_initialize();

void random_initialize();

void textmode_initialize();

void zero_initialize();

void serial_initialize();

void mouse_initialize();
