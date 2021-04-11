#include <libsystem/Logger.h>
#include <libsystem/utils/Hexdump.h>

#include "kernel/drivers/E1000.h"

void E1000::write_register(uint16_t offset, uint32_t value)
{
    if (!_mmio_range->empty())
    {
        _mmio_range->write32(offset, value);
    }
    else
    {
        out32(_pio_base, offset);
        out32(_pio_base + 4, value);
    }
}

uint32_t E1000::read_register(uint16_t offset)
{
    if (!_mmio_range->empty())
    {
        return _mmio_range->read32(offset);
    }
    else
    {
        out32(_pio_base, offset);
        return in32(_pio_base + 4);
    }
}

bool E1000::detect_eeprom()
{
    write_register(E1000_REG_EEPROM, 0x1);

    bool exists = false;

    for (int i = 0; i < 1000 && !exists; i++)
    {
        if (read_register(E1000_REG_EEPROM) & 0x10)
        {
            exists = true;
        }
    }

    return exists;
}

uint16_t E1000::read_eeprom(uint32_t address)
{
    uint16_t data = 0;
    uint32_t tmp = 0;

    if (_has_eeprom)
    {
        write_register(E1000_REG_EEPROM, (1) | ((uint32_t)(address) << 8));
        while (!((tmp = read_register(E1000_REG_EEPROM)) & (1 << 4)))
        {
        }
    }
    else
    {
        write_register(E1000_REG_EEPROM, (1) | ((uint32_t)(address) << 2));
        while (!((tmp = read_register(E1000_REG_EEPROM)) & (1 << 1)))
        {
        }
    }

    data = (uint16_t)((tmp >> 16) & 0xFFFF);
    return data;
}

MacAddress E1000::read_mac_address()
{
    MacAddress address{};

    if (_has_eeprom)
    {
        uint32_t temp;
        temp = read_eeprom(0);
        address.bytes[0] = temp & 0xff;
        address.bytes[1] = temp >> 8;
        temp = read_eeprom(1);
        address.bytes[2] = temp & 0xff;
        address.bytes[3] = temp >> 8;
        temp = read_eeprom(2);
        address.bytes[4] = temp & 0xff;
        address.bytes[5] = temp >> 8;
    }
    else
    {
        uint32_t mac_low = read_register(E1000_REG_MAC_LOW);
        uint32_t mac_hight = read_register(E1000_REG_MAC_HIGHT);

        address.bytes[0] = mac_low & 0xff;
        address.bytes[1] = mac_low >> 8 & 0xff;
        address.bytes[2] = mac_low >> 16 & 0xff;
        address.bytes[3] = mac_low >> 24 & 0xff;

        address.bytes[4] = mac_hight & 0xff;
        address.bytes[5] = mac_hight >> 8 & 0xff;
    }

    return address;
}

void E1000::initialize_rx()
{
    _rx_descriptors_range = make<MMIORange>(sizeof(E1000RXDescriptor) * E1000_NUM_RX_DESC);
    _rx_descriptors = reinterpret_cast<E1000RXDescriptor *>(_rx_descriptors_range->base());

    for (size_t i = 0; i < E1000_NUM_RX_DESC; i++)
    {
        _rx_buffers.push_back(make<MMIORange>(8192));
        _rx_descriptors[i].address = _rx_buffers[i]->physical_base();
        _rx_descriptors[i].status = 0x1;
    }

    write_register(E1000_REG_RX_LOW, _rx_descriptors_range->physical_base());
    write_register(E1000_REG_RX_HIGH, 0);
    write_register(E1000_REG_RX_LENGTH, E1000_NUM_RX_DESC * sizeof(E1000RXDescriptor));

    write_register(E1000_REG_RX_HEAD, 0);
    write_register(E1000_REG_RX_TAIL, E1000_NUM_RX_DESC - 1);
    write_register(E1000_REG_RX_CONTROL, RCTL_EN | RCTL_SBP | RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC | RCTL_BSIZE_8192);
}

void E1000::initialize_tx()
{
    _tx_descriptors_range = make<MMIORange>(sizeof(E1000TXDescriptor) * E1000_NUM_TX_DESC);
    _tx_descriptors = reinterpret_cast<E1000TXDescriptor *>(_tx_descriptors_range->base());

    for (size_t i = 0; i < E1000_NUM_TX_DESC; i++)
    {
        _tx_buffers.push_back(make<MMIORange>(8192));
        _tx_descriptors[i].address = _tx_buffers[i]->physical_base();
        _tx_descriptors[i].status = 0xff;
    }

    write_register(E1000_REG_TX_LOW, _tx_descriptors_range->physical_base());
    write_register(E1000_REG_TX_HIGH, 0);
    write_register(E1000_REG_TX_LENGTH, E1000_NUM_TX_DESC * sizeof(E1000TXDescriptor));

    write_register(E1000_REG_TX_HEAD, 0);
    write_register(E1000_REG_TX_TAIL, E1000_NUM_TX_DESC - 1);
    write_register(E1000_REG_TX_CONTROL, TCTL_EN | TCTL_PSP | (15 << TCTL_CT_SHIFT) | (64 << TCTL_COLD_SHIFT) | TCTL_RTLC);
}

void E1000::enable_interrupt()
{
    write_register(E1000_REG_IMASK, 0x1F6DC);
    write_register(E1000_REG_IMASK, 0xff & ~4);
    read_register(0xc0);
}

size_t E1000::receive_packet(void *buffer, size_t size)
{
    UNUSED(size);

    logger_trace("rx");

    uint32_t packet_size = _rx_descriptors[_current_rx_descriptors].length;
    _rx_buffers[_current_rx_descriptors]->read(0, buffer, packet_size);
    _rx_descriptors[_current_rx_descriptors].status = 0;

    uint16_t old_cur = _current_rx_descriptors;
    _current_rx_descriptors = (_current_rx_descriptors + 1) % E1000_NUM_RX_DESC;
    write_register(E1000_REG_RX_TAIL, old_cur);

    return packet_size;
}

size_t E1000::send_packet(const void *buffer, size_t size)
{
    logger_trace("tx");

    _tx_buffers[_current_tx_descriptors]->write(0, buffer, size);
    _tx_descriptors[_current_tx_descriptors].length = size;
    _tx_descriptors[_current_tx_descriptors].command = CMD_EOP | CMD_IFCS | CMD_RS;
    _tx_descriptors[_current_tx_descriptors].status = 0;

    _current_tx_descriptors = (_current_tx_descriptors + 1) % E1000_NUM_TX_DESC;
    write_register(E1000_REG_TX_TAIL, _current_tx_descriptors);

    return size;
}

E1000::E1000(DeviceAddress address) : PCIDevice(address, DeviceClass::NETWORK)
{
    auto bar0 = bar(0);

    if (bar0.type() == PCIBarType::MMIO32)
    {
        _mmio_range = make<MMIORange>(bar0.range());
    }
    else
    {
        _pio_base = bar0.base();
    }

    _has_eeprom = detect_eeprom();
    _mac_address = read_mac_address();

    initialize_rx();
    initialize_tx();
    enable_interrupt();
}

void E1000::acknowledge_interrupt()
{
    write_register(E1000_REG_IMASK, 0x0);
}

void E1000::handle_interrupt()
{
    uint32_t status = read_register(E1000_REG_STATUS);

    // logger_trace("e1000 interrupt (STATUS=%08x)!", status);

    if (status & 4)
    {
        uint32_t flags = read_register(E1000_REG_CONTROL);
        write_register(E1000_REG_CONTROL, flags | E1000_CTL_START_LINK);
    }
}

bool E1000::can_write()
{
    return (_tx_descriptors[_current_tx_descriptors].status & 1);
}

bool E1000::can_read()
{
    return _rx_descriptors[_current_rx_descriptors].status & 0x1;
}

ResultOr<size_t> E1000::read(size64_t offset, void *buffer, size_t size)
{
    UNUSED(offset);

    size_t packet_size = receive_packet(buffer, size);
    logger_trace("Packet receive (size=%u)", packet_size);
    hexdump(buffer, packet_size);

    return packet_size;
}

ResultOr<size_t> E1000::write(size64_t offset, const void *buffer, size_t size)
{
    UNUSED(offset);

    size_t packet_size = send_packet(buffer, size);
    logger_trace("Packet send (size=%u)", packet_size);
    hexdump(buffer, packet_size);

    return packet_size;
}

Result E1000::call(IOCall request, void *args)
{
    if (request == IOCALL_NETWORK_GET_STATE)
    {
        IOCallNetworkSateAgs *state = (IOCallNetworkSateAgs *)args;
        state->mac_address = _mac_address;
        return SUCCESS;
    }
    else
    {
        return ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }
}
