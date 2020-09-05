#include <abi/Mouse.h>
#include <abi/Paths.h>

#include <libsystem/Logger.h>
#include <libsystem/thread/Atomic.h>
#include <libsystem/utils/RingBuffer.h>

#include "arch/x86/PS2.h"
#include "arch/x86/x86.h"
#include "kernel/filesystem/Filesystem.h"
#include "kernel/interrupts/Dispatcher.h"

static RingBuffer *_mouse_buffer;
static int _mouse_cycle = 0;
static uint8_t _mouse_packet[4];

static void ps2mouse_handle_finished_packet(uint8_t packet0, uint8_t packet1, uint8_t packet2, uint8_t packet3)
{
    //TODO: Scroll whell not suported yet
    __unused(packet3);

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

    // decode the new mouse packet
    MousePacket event;

    event.offx = offx;
    event.offy = -offy;
    event.scroll = 0;
    event.middle = (MouseButtonState)((packet0 >> 2) & 1);
    event.right = (MouseButtonState)((packet0 >> 1) & 1);
    event.left = (MouseButtonState)((packet0)&1);

    atomic_begin();
    if (ringbuffer_write(_mouse_buffer, (const char *)&event, sizeof(MousePacket)) != sizeof(MousePacket))
    {
        logger_warn("Mouse buffer overflow!");
    }
    atomic_end();
}

void ps2mouse_handle_packet(uint8_t packet)
{
    switch (_mouse_cycle)
    {
    case 0:
        _mouse_packet[0] = packet;
        if (_mouse_packet[0] & 8)
        {
            _mouse_cycle++;
        }
        break;
    case 1:
        _mouse_packet[1] = packet;
        _mouse_cycle++;
        break;
    case 2:
        _mouse_packet[2] = packet;

        ps2mouse_handle_finished_packet(_mouse_packet[0], _mouse_packet[1], _mouse_packet[2], _mouse_packet[3]);
        _mouse_cycle = 0;
        break;
    }
}

void ps2mouse_interrupt_handler()
{
    uint8_t status = in8(PS2_STATUS);

    while (((status & PS2_WHICH_BUFFER) == PS2_MOUSE_BUFFER) &&
           (status & PS2_BUFFER_FULL))
    {
        uint8_t packet = in8(PS2_BUFFER);
        ps2mouse_handle_packet(packet);
        status = in8(PS2_STATUS);
    }
}

static inline void ps2mouse_wait(int a_type)
{
    uint time_out = 100000;
    if (a_type == 0)
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

static inline void ps2mouse_write(uint8_t a_write)
{
    ps2mouse_wait(1);
    out8(0x64, 0xD4);
    ps2mouse_wait(1);
    out8(0x60, a_write);
}

static inline uint8_t ps2mouse_read()
{
    ps2mouse_wait(0);
    return in8(0x60);
}

class Mouse : public FsNode
{
private:
    /* data */
public:
    Mouse() : FsNode(FILE_TYPE_DEVICE)
    {
    }

    ~Mouse() {}

    bool can_read(FsHandle *handle)
    {
        __unused(handle);

        // FIXME: make this atomic or something...
        return !ringbuffer_is_empty(_mouse_buffer);
    }

    ResultOr<size_t> read(FsHandle &handle, void *buffer, size_t size)
    {
        __unused(handle);

        // FIXME: use locks
        atomic_begin();
        size_t read = ringbuffer_read(_mouse_buffer, (char *)buffer, (size / sizeof(MousePacket)) * sizeof(MousePacket));
        atomic_end();

        return read;
    }
};

void mouse_initialize()
{
    uint8_t _status;

    // Enable the auxiliary mouse device
    ps2mouse_wait(1);
    out8(0x64, 0xA8);

    // Enable the interrupts
    ps2mouse_wait(1);
    out8(0x64, 0x20);
    ps2mouse_wait(0);
    _status = (in8(0x60) | 3);
    ps2mouse_wait(1);
    out8(0x64, 0x60);
    ps2mouse_wait(1);
    out8(0x60, _status);

    // Tell the mouse to use default settings
    ps2mouse_write(0xF6);
    ps2mouse_read(); //Acknowledge

    // Enable the mouse
    ps2mouse_write(0xF4);
    ps2mouse_read(); //Acknowledge

    // FIXME: try to enable mouse whell

    // Setup the mouse handler
    _mouse_buffer = ringbuffer_create(sizeof(MousePacket) * 256);
    dispatcher_register_handler(12, ps2mouse_interrupt_handler);

    filesystem_link_and_take_ref_cstring(MOUSE_DEVICE_PATH, new Mouse());
}
