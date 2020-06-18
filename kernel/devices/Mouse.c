#include <abi/Mouse.h>
#include <abi/Paths.h>

#include <libsystem/Atomic.h>
#include <libsystem/Logger.h>
#include <libsystem/utils/RingBuffer.h>

#include "arch/x86/x86.h"
#include "kernel/interrupts/Dispatcher.h"
#include "kernel/tasking.h"

static RingBuffer *_mouse_buffer;
static uchar _mouse_cycle = 0;
static ubyte _mouse_packet[4];

static void ps2mouse_handle_packet(ubyte packet0, ubyte packet1, ubyte packet2, ubyte packet3)
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

void ps2mouse_interrupt_handler(void)
{
    switch (_mouse_cycle)
    {
    case 0:
        _mouse_packet[0] = in8(0x60);
        if (_mouse_packet[0] & 8)
        {
            _mouse_cycle++;
        }
        break;
    case 1:
        _mouse_packet[1] = in8(0x60);
        _mouse_cycle++;
        break;
    case 2:
        _mouse_packet[2] = in8(0x60);

        ps2mouse_handle_packet(_mouse_packet[0], _mouse_packet[1], _mouse_packet[2], _mouse_packet[3]);
        _mouse_cycle = 0;
        break;
    }
}

static inline void ps2mouse_wait(uchar a_type)
{
    uint time_out = 100000;
    if (a_type == 0)
    {
        while (time_out--)
        {
            if ((in8(0x64) & 1) == 1)
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
            if ((in8(0x64) & 2) == 0)
            {
                return;
            }
        }
        return;
    }
}

static inline void ps2mouse_write(uchar a_write)
{
    ps2mouse_wait(1);
    out8(0x64, 0xD4);
    ps2mouse_wait(1);
    out8(0x60, a_write);
}

static inline uchar ps2mouse_read(void)
{
    ps2mouse_wait(0);
    return in8(0x60);
}

bool mouse_can_read(FsNode *node, FsHandle *handle)
{
    __unused(node);
    __unused(handle);

    // FIXME: make this atomic or something...
    return !ringbuffer_is_empty(_mouse_buffer);
}

static Result mouse_read(FsNode *node, FsHandle *handle, void *buffer, size_t size, size_t *read)
{
    __unused(node);
    __unused(handle);

    // FIXME: use locks
    atomic_begin();
    *read = ringbuffer_read(_mouse_buffer, (char *)buffer, (size / sizeof(MousePacket)) * sizeof(MousePacket));
    atomic_end();

    return SUCCESS;
}

void mouse_initialize(void)
{
    uchar _status;

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

    // try to enable mouse whell
    // TODO

    // Setup the mouse handler
    _mouse_buffer = ringbuffer_create(sizeof(MousePacket) * 256);
    dispatcher_register_handler(12, ps2mouse_interrupt_handler);

    FsNode *mouse_device = __create(FsNode);

    fsnode_init(mouse_device, FILE_TYPE_DEVICE);

    FSNODE(mouse_device)->read = (FsNodeReadCallback)mouse_read;
    FSNODE(mouse_device)->can_read = (FsNodeCanReadCallback)mouse_can_read;

    Path *mouse_device_path = path_create(MOUSE_DEVICE_PATH);
    filesystem_link_and_take_ref(mouse_device_path, mouse_device);
    path_destroy(mouse_device_path);
}
