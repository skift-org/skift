#include "kernel/cpu/cpu.h"
#include "kernel/cpu/irq.h"
#include "kernel/mouse.h"
#include "kernel/logger.h"

uint mouse_x;
uint mouse_y;

/* --- Private functions ---------------------------------------------------- */

uchar cycle = 0;
char packet[3];

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
            char offx = packet[1];
            char offy = packet[2];

            mouse_x += offx;
            mouse_y -= offy;

            // log("Mouse %d %d (X=%d, Y=%d)", (int)packet[1], (int)packet[2], mouse_x, mouse_y);

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
    _status = (inb(0x60) | 2);
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

    //Setup the mouse handler

    irq_register(12, mouse_irq);
}

void mouse_get_position(uint *outxpos, uint *outypos)
{
    *outxpos = mouse_x;
    *outypos = mouse_y;
}

// int mouse_set_position(uint xpos, uint ypos)
// {
// }
// 
// mouse_bstate_t mouse_button(mouse_button_t button)
// {
// }