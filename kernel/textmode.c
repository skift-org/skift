#include <skift/cstring.h>
#include <skift/math.h>
#include <skift/error.h>
#include <skift/logger.h>

#include "kernel/dev/vga.h"
#include "kernel/filesystem.h"
#include "kernel/textmode.h"

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

        return -ERR_SUCESS;
    }
    else if (request == TEXTMODE_IOCTL_SET_INFO)
    {
        textmode_info_t* info = (textmode_info_t*)args;
    
        vga_cursor(info->cursor_x, info->cursor_y);
        return -ERR_SUCESS;
    }
    else if (request == TEXTMODE_IOCTL_SET_CELL)
    {
        textmode_cell_info_t* cell = (textmode_cell_info_t*)args;

        vga_cell(cell->x, cell->y, VGA_ENTRY(cell->c, cell->fg, cell->bg));

        return -ERR_SUCESS;
    }
    else
    {
        return -ERR_INVALID_REQUEST_CODE;
    }

}

void textmode_setup(void)
{
    device_t textmode_device = {
        .write = textmode_device_write,
        .ioctl = textmode_device_ioctl,
    };

    FILESYSTEM_MKDEV("textmode", textmode_device);
}
