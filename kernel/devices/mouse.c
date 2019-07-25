/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/cstring.h>
#include <skift/atomic.h>

#include <hjert/devices/mouse.h>
#include <hjert/message.h>

#include "cpu/irq.h"
#include "processor.h"
#include "tasking.h"

#include "mouse.h"

static mouse_state_t oldmouse = {0};

/* --- Private functions ---------------------------------------------------- */

static void mouse_send_button_event(mouse_button_t btn, bool up)
{
    mouse_button_event_t event = (mouse_button_event_t){.button = btn};
    message_t msg;

    if (up)
    {
        msg = message(MOUSE_BUTTONUP, -1);
    }
    else
    {
        msg = message(MOUSE_BUTTONDOWN, -1);
    }

    message_set_payload(msg, event);
    task_messaging_broadcast(task_kernel(), MOUSE_CHANNEL, &msg);
}

static void mouse_handle_packet(ubyte packet0, ubyte packet1, ubyte packet2, ubyte packet3)
{
    //TODO: Scroll whell not suported yet
    UNUSED(packet3);

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
    mouse_state_t newmouse;

    newmouse.x = offx;
    newmouse.y = -offy;
    newmouse.scroll = 0;
    newmouse.middle = (packet0 >> 2) & 1;
    newmouse.right = (packet0 >> 1) & 1;
    newmouse.left = (packet0)&1;

    if (newmouse.x != 0 || newmouse.y != 0)
    {
        // The mouse move
        mouse_move_event_t event = {.offx = newmouse.x, .offy = newmouse.y};
        message_t move_event = message(MOUSE_MOVE, -1);
        message_set_payload(move_event, event);
        task_messaging_broadcast(task_kernel(), MOUSE_CHANNEL, &move_event);
    }

    if (newmouse.scroll != 0)
    {
        mouse_scroll_event_t event = {.off = newmouse.scroll};
        message_t scroll_event = message(MOUSE_MOVE, -1);
        message_set_payload(scroll_event, event);
        task_messaging_broadcast(task_kernel(), MOUSE_SCROLL, &scroll_event);
    }

    if (oldmouse.left != newmouse.left)
    {
        mouse_send_button_event(MOUSE_BUTTON_LEFT, oldmouse.left);
    }

    if (oldmouse.right != newmouse.right)
    {
        mouse_send_button_event(MOUSE_BUTTON_RIGHT, oldmouse.right);
    }

    if (oldmouse.middle != newmouse.middle)
    {
        mouse_send_button_event(MOUSE_BUTTON_MIDDLE, oldmouse.middle);
    }

    oldmouse = newmouse;
}

static uchar cycle = 0;
static ubyte packet[4];

reg32_t mouse_irq(reg32_t esp, processor_context_t *context)
{
    UNUSED(context);

    switch (cycle)
    {
    case 0:
        packet[0] = in8(0x60);
        if (packet[0] & 8)
        {
            cycle++;
        }
        break;
    case 1:
        packet[1] = in8(0x60);
        cycle++;
        break;
    case 2:
        packet[2] = in8(0x60);

        mouse_handle_packet(packet[0], packet[1], packet[2], packet[3]);
        cycle = 0;
        break;
    }

    return esp;
}

static inline void mouse_wait(uchar a_type) //unsigned char
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

static inline void mouse_write(uchar a_write) //unsigned char
{
    //Wait to be able to send a command
    mouse_wait(1);
    //Tell the mouse we are sending a command
    out8(0x64, 0xD4);
    //Wait for the final part
    mouse_wait(1);
    //Finally write
    out8(0x60, a_write);
}

static inline uchar mouse_read(void)
{
    //Get's response from mouse
    mouse_wait(0);
    return in8(0x60);
}

/* --- Public functions ----------------------------------------------------- */

void mouse_setup(void)
{
    uchar _status;

    // Enable the auxiliary mouse device
    mouse_wait(1);
    out8(0x64, 0xA8);

    // Enable the interrupts
    mouse_wait(1);
    out8(0x64, 0x20);
    mouse_wait(0);
    _status = (in8(0x60) | 3);
    mouse_wait(1);
    out8(0x64, 0x60);
    mouse_wait(1);
    out8(0x60, _status);

    // Tell the mouse to use default settings
    mouse_write(0xF6);
    mouse_read(); //Acknowledge

    // Enable the mouse
    mouse_write(0xF4);
    mouse_read(); //Acknowledge

    // try to enable mouse whell
    // TODO

    // Setup the mouse handler
    irq_register(12, mouse_irq);
}