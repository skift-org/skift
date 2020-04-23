#pragma once

#include <libsystem/runtime.h>

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

typedef bool (*DeviceDriverMatch)(DeviceInfo info);
typedef void (*DeviceDriverInitialize)(DeviceInfo info);

typedef struct
{
    const char *description;

    Bus bus;
    DeviceDriverMatch match;
    DeviceDriverInitialize initialize;
} DeviceDriver;

typedef IterationDecision (*DeviceIterateCallback)(void *target, DeviceInfo device);

void device_initialize(void);

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
