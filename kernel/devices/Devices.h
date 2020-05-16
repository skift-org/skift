#pragma once

#include <libsystem/Common.h>

typedef enum
{
    BUS_NONE,
    BUS_LEGACY,
    BUS_PCI,
} Bus;

typedef struct
{
    Bus bus;

    int device;
    int vendor_id;
    int device_id;
} DeviceInfo;

typedef bool (*DeviceDriverInfoMatch)(DeviceInfo info);
typedef void (*DeviceDriverInfoInitialize)(DeviceInfo info);

typedef struct
{
    const char *description;

    Bus bus;
    DeviceDriverInfoMatch match;
    DeviceDriverInfoInitialize initialize;
} DeviceDriverInfo;

typedef IterationDecision (*DeviceIterateCallback)(void *target, DeviceInfo device);

void device_initialize(void);

const DeviceDriverInfo *device_get_diver_info(DeviceInfo info);

void device_iterate(void *target, DeviceIterateCallback callback);

/* --- Devices -------------------------------------------------------------- */

bool bga_match(DeviceInfo info);
void bga_initialize(DeviceInfo info);

void keyboard_initialize(void);

void null_initialize(void);

void proc_initialize(void);

void random_initialize(void);

void textmode_initialize(void);

void zero_initialize(void);

void serial_initialize(void);

void mouse_initialize(void);
