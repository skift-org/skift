/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* vga.c: legacy vga textmode graphic.                                        */

#include "kernel/processor.h"

#include "kernel/dev/vga.h"

static u16 *buffer = (u16 *)VGA_FRAME_BUFFER;

void vga_cell(u32 x, u32 y, ushort entry)
{
    if (x < VGA_SCREEN_WIDTH)
    {
        if (y < VGA_SCREEN_WIDTH)
        {
            buffer[y * VGA_SCREEN_WIDTH + x] = (u16)entry;
        }
    }
}

void vga_cursor(s32 x, s32 y)
{
    s16 cursorLocation = y * VGA_SCREEN_WIDTH + x;

    outb(0x3D4, 14);                  // Tell the VGA board we are setting the high cursor byte.
    outb(0x3D5, (u8)(cursorLocation >> 8)); // Send the high cursor byte.
    outb(0x3D4, 15);                  // Tell the VGA board we are setting the low cursor byte.
    outb(0x3D5, (u8)(cursorLocation));      // Send the low cursor byte.
}

void vga_hide_cursor()
{
    vga_cursor(-1, -1);
}
