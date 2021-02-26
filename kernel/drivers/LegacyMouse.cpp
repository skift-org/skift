#include "kernel/drivers/LegacyMouse.h"

void LegacyMouse::wait(int type)
{
    int time_out = 100000;
    if (type == 0)
    {
        while (time_out--)
        {
            if ((in8(PS2_STATUS) & 1) == 1)
            {
                return;
            }
        }
        return;
    }
    else
    {
        while (time_out--)
        {
            if ((in8(PS2_STATUS) & 2) == 0)
            {
                return;
            }
        }
        return;
    }
}

void LegacyMouse::write_register(uint8_t value)
{
    wait(1);
    out8(0x64, 0xD4);
    wait(1);
    out8(0x60, value);
}

uint8_t LegacyMouse::read_register()
{
    wait(0);
    return in8(0x60);
}

void LegacyMouse::handle_finished_packet(uint8_t packet0, uint8_t packet1, uint8_t packet2, uint8_t packet3)
{
    int offx = packet1;

    if (offx && (packet0 & (1 << 4)))
    {
        offx -= 0x100;
    }

    int offy = packet2;

    if (offy && (packet0 & (1 << 5)))
    {
        offy -= 0x100;
    }

    int scroll = 0;

    if (!_quirk_no_mouse_whell)
    {
        scroll = (int8_t)packet3;
    }

    // decode the new mouse packet
    MousePacket event;

    event.offx = offx;
    event.offy = -offy;
    event.scroll = scroll;
    event.middle = (MouseButtonState)((packet0 >> 2) & 1);
    event.right = (MouseButtonState)((packet0 >> 1) & 1);
    event.left = (MouseButtonState)((packet0)&1);

    if (_events.write((const char *)&event, sizeof(MousePacket)) != sizeof(MousePacket))
    {
        logger_warn("Mouse buffer overflow!");
    }
}

void LegacyMouse::handle_packet(uint8_t packet)
{
    switch (_cycle)
    {
    case 0:
        _packet[0] = packet;
        if (_packet[0] & 8)
        {
            _cycle++;
        }
        break;
    case 1:
        _packet[1] = packet;
        _cycle++;
        break;
    case 2:
        _packet[2] = packet;

        if (_quirk_no_mouse_whell)
        {
            handle_finished_packet(_packet[0], _packet[1], _packet[2], _packet[3]);
            _cycle = 0;
        }
        else
        {
            _cycle++;
        }

        break;
    case 3:
        _packet[3] = packet;

        handle_finished_packet(_packet[0], _packet[1], _packet[2], _packet[3]);
        _cycle = 0;
    }
}

LegacyMouse::LegacyMouse(DeviceAddress address) : LegacyDevice(address, DeviceClass::MOUSE)
{
    uint8_t _status;

    // Enable the auxiliary mouse device
    wait(1);
    out8(0x64, 0xA8);

    // Enable the interrupts
    wait(1);
    out8(0x64, 0x20);
    wait(0);
    _status = (in8(0x60) | 3);
    wait(1);
    out8(0x64, 0x60);
    wait(1);
    out8(0x60, _status);

    // Tell the mouse to use default settings
    write_register(0xF6);
    read_register(); //Acknowledge

    // Enable the mouse
    write_register(0xF4);
    read_register(); //Acknowledge

    // Try to  enable the scrollwhell
    write_register(0xF2);
    read_register();

    read_register();
    write_register(0xF3);
    read_register();
    write_register(200);
    read_register();

    write_register(0xF3);
    read_register();
    write_register(100);
    read_register();

    write_register(0xF3);
    read_register();
    write_register(80);
    read_register();

    write_register(0xF2);
    read_register();
    uint8_t result = read_register();
    if (result == 3)
    {
        _quirk_no_mouse_whell = false;
    }
}

void LegacyMouse::handle_interrupt()
{
    uint8_t status = in8(PS2_STATUS);

    while (((status & PS2_WHICH_BUFFER) == PS2_MOUSE_BUFFER) &&
           (status & PS2_BUFFER_FULL))
    {
        uint8_t packet = in8(PS2_BUFFER);
        handle_packet(packet);
        status = in8(PS2_STATUS);
    }
}

bool LegacyMouse::can_read()
{
    return !_events.empty();
}

ResultOr<size_t> LegacyMouse::read(size64_t offset, void *buffer, size_t size)
{
    __unused(offset);

    return _events.read((char *)buffer, (size / sizeof(MousePacket)) * sizeof(MousePacket));
}
