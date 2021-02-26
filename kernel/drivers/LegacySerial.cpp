#include "kernel/drivers/LegacySerial.h"

LegacySerial::LegacySerial(DeviceAddress address) : LegacyDevice(address, DeviceClass::SERIAL)
{
    com_initialize(port());
}

void LegacySerial::handle_interrupt()
{
    LockHolder holder(_buffer_lock);

    uint8_t status = in8(port() + 2);

    if (status == 0b100)
    {
        char byte = com_getc(port());
        _buffer.put(byte);
    }
}

bool LegacySerial::can_read()
{
    return !_buffer.empty();
}

ResultOr<size_t> LegacySerial::read(size64_t offset, void *buffer, size_t size)
{
    __unused(offset);

    LockHolder holder(_buffer_lock);

    return _buffer.read((char *)buffer, size);
}

ResultOr<size_t> LegacySerial::write(size64_t offset, const void *buffer, size_t size)
{
    __unused(offset);

    LockHolder holder(_buffer_lock);

    return com_write(COM1, buffer, size);
}
