#include <skift/cstring.h>
#include <skift/math.h>
#include <skift/error.h>
#include <skift/logger.h>

#include <hjert/devices/textmode.h>

#include "kernel/filesystem.h"
#include "kernel/devices.h"

/* ---VGA textmode driver --------------------------------------------------- */

#define VGA_FRAME_BUFFER 0XB8000

#define VGA_SCREEN_WIDTH 80
#define VGA_SCREEN_HEIGHT 25

#define VGA_ENTRY(__c, __fg, __bg) ((((__bg) & 0XF) << 4 | ((__fg) & 0XF)) << 8 | ((__c) & 0XFF))

void vga_cell(u32 x, u32 y, ushort entry)
{
    if (x < VGA_SCREEN_WIDTH)
    {
        if (y < VGA_SCREEN_HEIGHT)
        {
            ((u16 *)VGA_FRAME_BUFFER)[y * VGA_SCREEN_WIDTH + x] = (u16)entry;
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

/* --- Textmode abstract driver --------------------------------------------- */

int textmode_device_write(stream_t *stream, const void* buffer, uint size)
{
    UNUSED(stream);

    int tocopy = min(size, VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGHT * sizeof(short));

    memcpy((void*)VGA_FRAME_BUFFER, buffer, tocopy);

    return tocopy;
}

int textmode_device_ioctl(stream_t *stream, int request, void* args)
{
    UNUSED(stream);
   
    if (request == TEXTMODE_IOCTL_GET_INFO)
    {
        textmode_info_t* info = (textmode_info_t*)args;

        info->cursor_x = 0;
        info->cursor_y = 0;

        info->width = 80;
        info->height = 25;

        return -ERR_SUCCESS;
    }
    else if (request == TEXTMODE_IOCTL_SET_INFO)
    {
        textmode_info_t* info = (textmode_info_t*)args;
    
        vga_cursor(info->cursor_x, info->cursor_y);
        return -ERR_SUCCESS;
    }
    else if (request == TEXTMODE_IOCTL_SET_CELL)
    {
        textmode_cell_info_t* cell = (textmode_cell_info_t*)args;

        vga_cell(cell->x, cell->y, VGA_ENTRY(cell->c, cell->fg, cell->bg));

        return -ERR_SUCCESS;
    }
    else
    {
        return -ERR_INAPPROPRIATE_IOCTL_FOR_DEVICE;
    }

}

void textmode_setup(void)
{
    device_t textmode_device = {
        .write = textmode_device_write,
        .ioctl = textmode_device_ioctl,
    };

    FILESYSTEM_MKDEV(TEXTMODE_DEVICE, textmode_device);
}
