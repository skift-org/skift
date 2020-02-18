/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libdevice/textmode.h>
#include <libmath/MinMax.h>
#include <libsystem/Result.h>
#include <libsystem/atomic.h>
#include <libsystem/cstring.h>
#include <libsystem/logger.h>

#include "kernel/device/Device.h"
#include "kernel/filesystem/Filesystem.h"

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

void vga_cursor_enable()
{
    out8(0x3D4, 0x0A);
    out8(0x3D5, (in8(0x3D5) & 0xC0) | 0);

    out8(0x3D4, 0x0B);
    out8(0x3D5, (in8(0x3D5) & 0xE0) | 15);
}

void vga_cursor_disable()
{
    out8(0x3D4, 0x0A);
    out8(0x3D5, 0x20);
}

void vga_cursor_position(s32 x, s32 y)
{
    u16 cursorLocation = y * VGA_SCREEN_WIDTH + x;

    out8(0x3D4, 0x0F);
    out8(0x3D5, (uint8_t)(cursorLocation & 0xFF));
    out8(0x3D4, 0x0E);
    out8(0x3D5, (uint8_t)((cursorLocation >> 8) & 0xFF));
}

/* --- Textmode abstract driver --------------------------------------------- */

Result textmode_FsOperationWrite(FsNode *node, FsHandle *handle, const void *buffer, size_t size, size_t *writen)
{
    __unused(node);
    __unused(handle);

    size_t tocopy = MIN(size, VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGHT * sizeof(short));

    memcpy((void *)VGA_FRAME_BUFFER, buffer, tocopy);

    *writen = tocopy;

    return SUCCESS;
}

Result textmode_FsOperationCall(FsNode *node, FsHandle *handle, int request, void *args)
{
    __unused(node);
    __unused(handle);

    if (request == TEXTMODE_CALL_GET_INFO)
    {
        textmode_info_t *info = (textmode_info_t *)args;

        info->cursor_x = 0;
        info->cursor_y = 0;

        info->width = 80;
        info->height = 25;

        return SUCCESS;
    }
    else if (request == TEXTMODE_CALL_SET_INFO)
    {
        textmode_info_t *info = (textmode_info_t *)args;

        vga_cursor_position(info->cursor_x, info->cursor_y);
        vga_cursor_enable();

        return SUCCESS;
    }
    else if (request == TEXTMODE_CALL_SET_CELL)
    {
        textmode_cell_info_t *cell = (textmode_cell_info_t *)args;

        vga_cell(cell->x, cell->y, VGA_ENTRY(cell->c, cell->fg, cell->bg));

        return SUCCESS;
    }
    else
    {
        return ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }
}

void textmode_initialize(void)
{
    logger_info("Initializing textmode graphic");

    FsNode *textmode_device = __create(FsNode);
    fsnode_init(textmode_device, FSNODE_DEVICE);

    FSNODE(textmode_device)->write = (FsOperationWrite)textmode_FsOperationWrite;
    FSNODE(textmode_device)->call = (FsOperationCall)textmode_FsOperationCall;

    Path *textmode_device_path = path_create(TEXTMODE_DEVICE);
    filesystem_link_and_take_ref(textmode_device_path, textmode_device);
    path_destroy(textmode_device_path);
}
