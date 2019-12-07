#include <libsystem/cstring.h>
#include <libmath/math.h>
#include <libsystem/error.h>
#include <libsystem/logger.h>
#include <libsystem/atomic.h>

#include <libdevice/textmode.h>

#include "processor.h"
#include "filesystem.h"
#include "devices.h"

/* ---VGA textmode driver --------------------------------------------------- */

#define VGA_FRAME_BUFFER 0XB8000

#define VGA_SCREEN_WIDTH 80
#define VGA_SCREEN_HEIGHT 25

#define VGA_ENTRY(__c, __fg, __bg) ((((__bg)&0XF) << 4 | ((__fg)&0XF)) << 8 | ((__c)&0XFF))

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

void vga_cursor_enable(u8 cursor_start, u8 cursor_end)
{
	out8(0x3D4, 0x0A);
	out8(0x3D5, (in8(0x3D5) & 0xC0) | cursor_start);
 
	out8(0x3D4, 0x0B);
	out8(0x3D5, (in8(0x3D5) & 0xE0) | cursor_end);
}

void vga_cursor_disable()
{
    out8(0x3D4, 0x0A);
	out8(0x3D5, 0x20);
}

void vga_cursor_position(s32 x, s32 y)
{
    u16 cursorLocation = y * VGA_SCREEN_WIDTH + x;

    out8(0x3D4, 14);                        // Tell the VGA board we are setting the high cursor byte.
    out8(0x3D5, (u8)(cursorLocation));      // Send the low cursor byte.
    out8(0x3D4, 15);                        // Tell the VGA board we are setting the low cursor byte.
    out8(0x3D5, (u8)(cursorLocation >> 8)); // Send the high cursor byte.
}

/* --- Textmode abstract driver --------------------------------------------- */

int textmode_device_write(stream_t *stream, const void *buffer, uint size)
{
    __unused(stream);

    int tocopy = min(size, VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGHT * sizeof(short));

    memcpy((void *)VGA_FRAME_BUFFER, buffer, tocopy);

    return tocopy;
}

int textmode_device_call(stream_t *stream, int request, void *args)
{
    __unused(stream);

    if (request == TEXTMODE_CALL_GET_INFO)
    {
        textmode_info_t *info = (textmode_info_t *)args;

        info->cursor_x = 0;
        info->cursor_y = 0;

        info->width = 80;
        info->height = 25;

        return -ERR_SUCCESS;
    }
    else if (request == TEXTMODE_CALL_SET_INFO)
    {
        textmode_info_t *info = (textmode_info_t *)args;

        vga_cursor_position(info->cursor_x, info->cursor_y);
        return -ERR_SUCCESS;
    }
    else if (request == TEXTMODE_CALL_SET_CELL)
    {
        textmode_cell_info_t *cell = (textmode_cell_info_t *)args;

        vga_cell(cell->x, cell->y, VGA_ENTRY(cell->c, cell->fg, cell->bg));

        return -ERR_SUCCESS;
    }
    else
    {
        return -ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }
}

void textmode_setup(void)
{
    device_t textmode_device = {
        .write = textmode_device_write,
        .call = textmode_device_call,
    };

    FILESYSTEM_MKDEV(TEXTMODE_DEVICE, textmode_device);
}
