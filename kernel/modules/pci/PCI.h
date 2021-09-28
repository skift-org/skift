#pragma once

#include <libutils/Func.h>

#include "system/devices/DeviceAddress.h"

void pci_initialize();

int pci_get_interrupt(PCIAddress address);

Iter pci_scan(IterFunc<PCIAddress> callback);
