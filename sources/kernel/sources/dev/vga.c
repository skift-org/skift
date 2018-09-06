// This file is part of "skiftOS" licensed under the MIT License.
// See: LICENSE.md
// Project URL: github.com/maker-dev/skift

#include "kernel/dev/vga.h"
#include "kernel/cpu/cpu.h"

u16 *video_memory = (u16 *)vga_frame_buffer;

void vga_setup()
{
    vga_clear(vga_white, vga_black);
    vga_cursor(0, 0);
}

void vga_clear(vga_color_t fg, vga_color_t bg)
{
    for (u32 x = 0; x < vga_screen_width; x++)
    {
        for (u32 y = 0; y < vga_screen_height + 1; y++)
        {
            video_memory[y * vga_screen_width + x] = vga_entry(' ', fg, bg);
        }
    }
}

void vga_scroll(vga_color_t bg)
{
    for (u32 x = 0; x < vga_screen_width; x++)
    {
        for (u32 y = 0; y < vga_screen_height + 1; y++)
        {
            video_memory[y * vga_screen_width + x] = video_memory[(y + 1) * vga_screen_width + x];
        }
    }

    u16 entry = vga_entry(' ', vga_black, bg);

    for (u32 i = 0; i < vga_screen_width; i++)
    {
        video_memory[((vga_screen_height - 1) * vga_screen_width) + i] = entry;
    }
}

void vga_cell(u32 x, u32 y, vga_entry_t entry)
{
    if (x < vga_screen_width)
    {
        if (y < vga_screen_height)
        {
            video_memory[y * vga_screen_width + x] = (u16)entry;
        }
    }
}

void vga_text(u32 x, u32 y, string text, vga_color_t fg, vga_color_t bg)
{

    for(size_t i = 0; text[i]; i++)
    {
        vga_cell(x + i, y, vga_entry(text[i], fg, bg));
    }

}

void vga_cursor(s32 x, s32 y)
{
    s16 cursorLocation = y * vga_screen_width + x;
    outb(0x3D4, 14);                  // Tell the VGA board we are setting the high cursor byte.
    outb(0x3D5, cursorLocation >> 8); // Send the high cursor byte.
    outb(0x3D4, 15);                  // Tell the VGA board we are setting the low cursor byte.
    outb(0x3D5, cursorLocation);      // Send the low cursor byte.
}

void vga_hide_cursor()
{
    vga_cursor(-1, -1);
}
