/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>
#include <skift/atomic.h>

#include "kernel/cpu/cpu.h"
#include "kernel/cpu/irq.h"
#include "kernel/logger.h"

#include "kernel/mouse.h"

mouse_state_t mouse;

/* --- Private functions ---------------------------------------------------- */

void mouse_handle_packet(ubyte packet0, ubyte packet1, ubyte packet2, ubyte packet3)
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

    mouse.x += offx;
    mouse.y -= offy;
    mouse.scroll = 0;

    mouse.middle = (packet0 >> 2) & 1;
    mouse.right = (packet0 >> 1) & 1;
    mouse.left = (packet0)&1;

    // log("Mouse %d %d (X=%d, Y=%d, L=%d, M=%d, R=%d)", (int)offx, (int)offy, mouse.x, mouse.y, mouse.left, mouse.center, mouse.right);
}

uchar cycle = 0;
ubyte packet[4];
esp_t mouse_irq(esp_t esp, context_t *context)
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

inline void mouse_wait(uchar a_type) //unsigned char
{
    uint _time_out = 100000; //unsigned int
    if (a_type == 0)
    {
        while (_time_out--) //Data
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
        while (_time_out--) //Signal
        {
            if ((inb(0x64) & 2) == 0)
            {
                return;
            }
        }
        return;
    }
}

inline void mouse_write(uchar a_write) //unsigned char
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

uchar mouse_read()
{
    //Get's response from mouse
    mouse_wait(0);
    return inb(0x60);
}

/* --- Public functions ----------------------------------------------------- */

void mouse_setup()
{
    uchar _status; //unsigned char

    //Enable the auxiliary mouse device
    mouse_wait(1);
    outb(0x64, 0xA8);

    //Enable the interrupts
    mouse_wait(1);
    outb(0x64, 0x20);
    mouse_wait(0);
    _status = (inb(0x60) | 3);
    mouse_wait(1);
    outb(0x64, 0x60);
    mouse_wait(1);
    outb(0x60, _status);

    //Tell the mouse to use default settings
    mouse_write(0xF6);
    mouse_read(); //Acknowledge

    //Enable the mouse
    mouse_write(0xF4);
    mouse_read(); //Acknowledge

    // try to enable mouse whell

    //Setup the mouse handler
    irq_register(12, mouse_irq);
}

void mouse_get_state(mouse_state_t *state)
{
    ATOMIC({
        memcpy(state, &mouse, sizeof(mouse_state_t));
    });
}

void mouse_set_state(mouse_state_t *state)
{
    ATOMIC({
        memcpy(&mouse, state, sizeof(mouse_state_t));
    });
}