#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

#define PCI_VENDOR_ID 0x00
#define PCI_DEVICE_ID 0x02
#define PCI_COMMAND 0x04
#define PCI_STATUS 0x06
#define PCI_REVISION_ID 0x08

#define PCI_PROG_IF 0x09
#define PCI_SUBCLASS 0x0a
#define PCI_CLASS 0x0b
#define PCI_CACHE_LINE_SIZE 0x0c
#define PCI_LATENCY_TIMER 0x0d
#define PCI_HEADER_TYPE 0x0e
#define PCI_BIST 0x0f
#define PCI_BAR0 0x10
#define PCI_BAR1 0x14
#define PCI_BAR2 0x18
#define PCI_BAR3 0x1C
#define PCI_BAR4 0x20
#define PCI_BAR5 0x24

#define PCI_INTERRUPT_LINE 0x3C

#define PCI_SECONDARY_BUS 0x19

#define PCI_HEADER_TYPE_DEVICE 0
#define PCI_HEADER_TYPE_BRIDGE 1
#define PCI_HEADER_TYPE_CARDBUS 2

#define PCI_TYPE_BRIDGE 0x0604
#define PCI_TYPE_SATA 0x0106

#define PCI_ADDRESS_PORT 0xCF8
#define PCI_VALUE_PORT 0xCFC

#define PCI_NONE 0xFFFF

typedef void (*pci_func_t)(uint32_t device, uint16_t vendor_id, uint16_t device_id, void *extra);

static inline int pci_extract_bus(uint32_t device)
{
	return (uint8_t)((device >> 16));
}
static inline int pci_extract_slot(uint32_t device)
{
	return (uint8_t)((device >> 8));
}
static inline int pci_extract_func(uint32_t device)
{
	return (uint8_t)(device);
}

static inline uint32_t pci_get_addr(uint32_t device, int field)
{
	return 0x80000000 | (pci_extract_bus(device) << 16) | (pci_extract_slot(device) << 11) | (pci_extract_func(device) << 8) | ((field)&0xFC);
}

static inline uint32_t pci_box_device(int bus, int slot, int func)
{
	return (uint32_t)((bus << 16) | (slot << 8) | func);
}

uint32_t pci_read_field(uint32_t device, int field, int size);

void pci_write_field(uint32_t device, int field, int size, uint32_t value);

uint16_t pci_find_type(uint32_t dev);

void pci_scan_hit(pci_func_t f, uint32_t dev, void *extra);

void pci_scan_func(pci_func_t f, int type, int bus, int slot, int func, void *extra);

void pci_scan_slot(pci_func_t f, int type, int bus, int slot, void *extra);

void pci_scan_bus(pci_func_t f, int type, int bus, void *extra);

void pci_scan(pci_func_t f, int type, void *extra);

void pci_remap(void);

int pci_get_interrupt(uint32_t device);
