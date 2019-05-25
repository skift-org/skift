/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/cstring.h>
#include <skift/atomic.h>

#include <hjert/dev/mouse.h>
#include <hjert/shared/message.h>

#include <hjert/cpu/irq.h>
#include <hjert/processor.h>
#include <hjert/tasking.h>

#include <hjert/mouse.h>

static mouse_state_t oldmouse = {0};

/* --- Private functions ---------------------------------------------------- */

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
        messaging_broadcast(MOUSE_CHANNEL, MOUSE_MOVE, &event, sizeof(mouse_move_event_t), 0);
    }

    if (newmouse.scroll != 0)
    {
        mouse_scroll_event_t event = {.off = newmouse.scroll};
        messaging_broadcast(MOUSE_CHANNEL, MOUSE_SCROLL, &event, sizeof(mouse_scroll_event_t), 0);
    }

    if (oldmouse.left != newmouse.left)
    {
        if (oldmouse.left == false)
        {
            mouse_button_event_t event = {.button = MOUSE_BUTTON_LEFT};
            messaging_broadcast(MOUSE_CHANNEL, MOUSE_BUTTONDOWN, &event, sizeof(mouse_button_event_t), 0);
        }
        else
        {
            mouse_button_event_t event = {.button = MOUSE_BUTTON_LEFT};
            messaging_broadcast(MOUSE_CHANNEL, MOUSE_BUTTONUP, &event, sizeof(mouse_button_event_t), 0);
        }
    }

    if (oldmouse.right != newmouse.right)
    {
        if (oldmouse.right == false)
        {
            mouse_button_event_t event = {.button = MOUSE_BUTTON_RIGHT};
            messaging_broadcast(MOUSE_CHANNEL, MOUSE_BUTTONDOWN, &event, sizeof(mouse_button_event_t), 0);
        }
        else
        {
            mouse_button_event_t event = {.button = MOUSE_BUTTON_RIGHT};
            messaging_broadcast(MOUSE_CHANNEL, MOUSE_BUTTONUP, &event, sizeof(mouse_button_event_t), 0);
        }
    }

    if (oldmouse.middle != newmouse.middle)
    {
        if (oldmouse.middle == false)
        {
            mouse_button_event_t event = {.button = MOUSE_BUTTON_MIDDLE};
            messaging_broadcast(MOUSE_CHANNEL, MOUSE_BUTTONDOWN, &event, sizeof(mouse_button_event_t), 0);
        }
        else
        {
            mouse_button_event_t event = {.button = MOUSE_BUTTON_MIDDLE};
            messaging_broadcast(MOUSE_CHANNEL, MOUSE_BUTTONUP, &event, sizeof(mouse_button_event_t), 0);
        }
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
        packet[0] = inb(0x60);
        if (packet[0] & 8)
        {
            cycle++;
        }
        break;
    case 1:
        packet[1] = inb(0x60);
        cycle++;
        break;
    case 2:
        packet[2] = inb(0x60);

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
            if ((inb(0x64) & 1) == 1)
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
            if ((inb(0x64) & 2) == 0)
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
    outb(0x64, 0xD4);
    //Wait for the final part
    mouse_wait(1);
    //Finally write
    outb(0x60, a_write);
}

static inline uchar mouse_read(void)
{
    //Get's response from mouse
    mouse_wait(0);
    return inb(0x60);
}

/* --- Public functions ----------------------------------------------------- */

void mouse_setup(void)
{
    uchar _status;

    // Enable the auxiliary mouse device
    mouse_wait(1);
    outb(0x64, 0xA8);

    // Enable the interrupts
    mouse_wait(1);
    outb(0x64, 0x20);
    mouse_wait(0);
    _status = (inb(0x60) | 3);
    mouse_wait(1);
    outb(0x64, 0x60);
    mouse_wait(1);
    outb(0x60, _status);

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