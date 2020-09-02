#include <libsystem/Logger.h>
#include <libsystem/net/MacAddress.h>

#include "arch/x86/x86.h"
#include "kernel/bus/PCI.h"
#include "kernel/devices/Devices.h"
#include "kernel/devices/E1000.h"
#include "kernel/devices/MMIO.h"
#include "kernel/memory/MemoryRange.h"
#include "kernel/memory/Physical.h"
#include "kernel/memory/Virtual.h"

static MemoryRange _mmio_range = {};
static uint16_t _pio_base = 0;
static bool _has_eeprom = false;
static MacAddress _mac_address = {};

int _current_rx_descriptors = 0;
static MemoryRange _rx_descriptors_range = {};
static E1000RXDescriptor *_rx_descriptors = {};

int _current_tx_descriptors = 0;
static MemoryRange _tx_descriptors_range = {};
static E1000TXDescriptor *_tx_descriptors = {};

static void e1000_write(uint16_t offset, uint32_t value)
{
    if (!_mmio_range.empty())
    {
        mmio_write32(_mmio_range.base() + offset, value);
    }
    else
    {
        out32(_pio_base, offset);
        out32(_pio_base + 4, value);
    }
}

static uint32_t e1000_read(uint16_t offset)
{
    if (!_mmio_range.empty())
    {
        return mmio_read32(_mmio_range.base() + offset);
    }
    else
    {
        out32(_pio_base, offset);
        return in32(_pio_base + 4);
    }
}

/* --- eeprom --------------------------------------------------------------- */

bool e1000_eeprom_detect()
{
    e1000_write(E1000_REG_EEPROM, 0x1);

    bool exists = false;

    for (int i = 0; i < 1000 && !exists; i++)
    {
        if (e1000_read(E1000_REG_EEPROM) & 0x10)
        {
            exists = true;
        }
    }

    return exists;
}

uint16_t e1000_eeprom_read(uint32_t address)
{
    uint16_t data = 0;
    uint32_t tmp = 0;
    if (_has_eeprom)
    {
        e1000_write(E1000_REG_EEPROM, (1) | ((uint32_t)(address) << 8));
        while (!((tmp = e1000_read(E1000_REG_EEPROM)) & (1 << 4)))
            ;
    }
    else
    {
        e1000_write(E1000_REG_EEPROM, (1) | ((uint32_t)(address) << 2));
        while (!((tmp = e1000_read(E1000_REG_EEPROM)) & (1 << 1)))
            ;
    }
    data = (uint16_t)((tmp >> 16) & 0xFFFF);
    return data;
}

/* --- Mac Address ---------------------------------------------------------- */

MacAddress e1000_mac_address_read()
{
    MacAddress address{};

    if (_has_eeprom)
    {
        uint32_t temp;
        temp = e1000_eeprom_read(0);
        address.bytes[0] = temp & 0xff;
        address.bytes[1] = temp >> 8;
        temp = e1000_eeprom_read(1);
        address.bytes[2] = temp & 0xff;
        address.bytes[3] = temp >> 8;
        temp = e1000_eeprom_read(2);
        address.bytes[4] = temp & 0xff;
        address.bytes[5] = temp >> 8;
    }
    else
    {
        uint32_t mac_low = e1000_read(E1000_REG_MAC_LOW);
        uint32_t mac_hight = e1000_read(E1000_REG_MAC_HIGHT);

        address.bytes[0] = mac_low & 0xff;
        address.bytes[1] = mac_low >> 8 & 0xff;
        address.bytes[2] = mac_low >> 16 & 0xff;
        address.bytes[3] = mac_low >> 24 & 0xff;

        address.bytes[4] = mac_hight & 0xff;
        address.bytes[5] = mac_hight >> 8 & 0xff;
    }

    return address;
}

/* --- Rx/Tx ---------------------------------------------------------------- */

void e1000_initialize_rx()
{
    _rx_descriptors_range = physical_alloc(PAGE_ALIGN_UP(sizeof(E1000RXDescriptor) * E1000_NUM_RX_DESC));
    _rx_descriptors = (E1000RXDescriptor *)virtual_alloc(&kpdir, _rx_descriptors_range, MEMORY_NONE).base();

    for (size_t i = 0; i < E1000_NUM_RX_DESC; i++)
    {
        uintptr_t rx_buffer = 0;
        memory_alloc(&kpdir, 8192, MEMORY_NONE, &rx_buffer);

        _rx_descriptors[i].address = virtual_to_physical(&kpdir, rx_buffer);
        _rx_descriptors[i].status = 0;
    }

    e1000_write(E1000_REG_RX_LOW, _rx_descriptors_range.base());
    e1000_write(E1000_REG_RX_HIGH, 0);
    e1000_write(E1000_REG_RX_LENGTH, E1000_NUM_RX_DESC * sizeof(E1000RXDescriptor));

    e1000_write(E1000_REG_RX_HEAD, 0);
    e1000_write(E1000_REG_RX_TAIL, E1000_NUM_RX_DESC - 1);
    e1000_write(E1000_REG_RX_CONTROL, RCTL_EN | RCTL_SBP | RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC | RCTL_BSIZE_8192);
}

void e1000_initialize_tx()
{
    _tx_descriptors_range = physical_alloc(PAGE_ALIGN_UP(sizeof(E1000TXDescriptor) * E1000_NUM_TX_DESC));
    _tx_descriptors = (E1000TXDescriptor *)virtual_alloc(&kpdir, _tx_descriptors_range, MEMORY_NONE).base();

    for (size_t i = 0; i < E1000_NUM_TX_DESC; i++)
    {
        uintptr_t tx_buffer = 0;
        memory_alloc(&kpdir, 8192, MEMORY_NONE, &tx_buffer);

        _tx_descriptors[i].address = virtual_to_physical(&kpdir, tx_buffer);
        _tx_descriptors[i].status = 0;
    }

    e1000_write(E1000_REG_TX_LOW, _tx_descriptors_range.base());
    e1000_write(E1000_REG_TX_HIGH, 0);
    e1000_write(E1000_REG_TX_LENGTH, E1000_NUM_TX_DESC * sizeof(E1000TXDescriptor));

    e1000_write(E1000_REG_TX_HEAD, 0);
    e1000_write(E1000_REG_TX_TAIL, E1000_NUM_TX_DESC - 1);
    e1000_write(E1000_REG_TX_CONTROL, TCTL_EN | TCTL_PSP | (15 << TCTL_CT_SHIFT) | (64 << TCTL_COLD_SHIFT) | TCTL_RTLC);
}

void e1000_enable_interrupt()
{
    e1000_write(E1000_REG_IMASK, 0x1F6DC);
    e1000_write(E1000_REG_IMASK, 0xff & ~4);
    e1000_read(0xc0);
}

/* --- device --------------------------------------------------------------- */

bool e1000_match(DeviceInfo info)
{
    return info.pci_device.vendor == 0x8086 &&
           (info.pci_device.device == 0x100E || //  Qemu, Bochs, and VirtualBox emmulated NICs
            info.pci_device.device == 0x153A || // Intel I217
            info.pci_device.device == 0x153A);  //  Intel 82577LM
}

void e1000_initialize(DeviceInfo info)
{
    if (pci_device_type_bar(info.pci_device, 0) == PCIBarType::MMIO32)
    {
        uintptr_t memory_base = pci_device_read_bar(info.pci_device, 0) & 0xFFFFFFF0;
        size_t memory_size = pci_device_size_bar(info.pci_device, 0);

        _mmio_range = virtual_alloc(&kpdir, MemoryRange{memory_base, memory_size}, MEMORY_NONE);
    }
    else
    {
        _pio_base = pci_device_read_bar(info.pci_device, 0) & 0xFFFFFFFC;
    }

    _has_eeprom = e1000_eeprom_detect();
    _mac_address = e1000_mac_address_read();

    e1000_initialize_rx();
    e1000_initialize_tx();
    e1000_enable_interrupt();

    logger_trace("Mac address is %02x:%02x:%02x:%02x:%02x:%02x",
                 _mac_address[0], _mac_address[1], _mac_address[2], _mac_address[3], _mac_address[4], _mac_address[5]);
}
