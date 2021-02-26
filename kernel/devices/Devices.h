#pragma once

#include <libsystem/Common.h>
#include <libutils/Callback.h>

#include "kernel/devices/DeviceAddress.h"
#include "kernel/devices/DeviceClass.h"
#include "kernel/devices/Driver.h"

void device_scan(IterationCallback<DeviceAddress> address);

void device_iterate(IterationCallback<RefPtr<Device>> callback);

String device_claim_name(DeviceClass klass);

void device_initialize();

void devices_acknowledge_interrupt(int interrupt);

void devices_handle_interrupt(int interrupt);

void device_mount(RefPtr<Device> device);
