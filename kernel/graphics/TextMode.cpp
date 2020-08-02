#include <abi/Paths.h>
#include <abi/TextMode.h>

#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/core/CString.h>
#include <libsystem/math/MinMax.h>
#include <libsystem/thread/Atomic.h>

#include "arch/x86/x86.h"

#include "kernel/devices/Devices.h"
#include "kernel/filesystem/Filesystem.h"
#include "kernel/memory/Virtual.h"

/* ---VGA textmode driver --------------------------------------------------- */

#define VGA_FRAME_BUFFER 0XB8000

static uint16_t *_text_buffer = nullptr;

#define VGA_SCREEN_WIDTH 80
#define VGA_SCREEN_HEIGHT 25

#define VGA_ENTRY(__c, __fg, __bg) ((((__bg)&0XF) << 4 | ((__fg)&0XF)) << 8 | ((__c)&0XFF))

void vga_cell(u32 x, u32 y, uint16_t entry)
{
    if (x < VGA_SCREEN_WIDTH)
    {
        if (y < VGA_SCREEN_HEIGHT)
        {
            ((u16 *)_text_buffer)[y * VGA_SCREEN_WIDTH + x] = (u16)entry;
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

void vga_cursor_position(int x, int y)
{
    int cursorLocation = y * VGA_SCREEN_WIDTH + x;

    out8(0x3D4, 0x0F);
    out8(0x3D5, (uint8_t)(cursorLocation & 0xFF));
    out8(0x3D4, 0x0E);
    out8(0x3D5, (uint8_t)((cursorLocation >> 8) & 0xFF));
}

MemoryRange vga_physical_range()
{
    return (MemoryRange){VGA_FRAME_BUFFER, PAGE_ALIGN_UP(VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGHT * sizeof(uint16_t))};
}

/* --- Textmode abstract driver --------------------------------------------- */

Result textmode_write(FsNode *node, FsHandle *handle, const void *buffer, size_t size, size_t *written)
{
    __unused(node);
    __unused(handle);

    size_t tocopy = MIN(size, VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGHT * sizeof(short));

    memcpy((void *)_text_buffer, buffer, tocopy);

    *written = tocopy;

    return SUCCESS;
}

Result textmode_iocall(FsNode *node, FsHandle *handle, IOCall request, void *args)
{
    __unused(node);
    __unused(handle);

    if (request == IOCALL_TEXTMODE_GET_STATE)
    {
        IOCallTextModeStateArgs *info = (IOCallTextModeStateArgs *)args;

        info->cursor_x = 0;
        info->cursor_y = 0;

        info->width = 80;
        info->height = 25;

        return SUCCESS;
    }
    else if (request == IOCALL_TEXTMODE_SET_STATE)
    {
        IOCallTextModeStateArgs *info = (IOCallTextModeStateArgs *)args;

        vga_cursor_position(info->cursor_x, info->cursor_y);
        vga_cursor_enable();

        return SUCCESS;
    }
    else
    {
        return ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }
}

void textmode_initialize()
{
    logger_info("Initializing textmode graphic");

    _text_buffer = (uint16_t *)virtual_alloc(&kpdir, vga_physical_range(), MEMORY_NONE).base;

    FsNode *textmode_device = __create(FsNode);
    fsnode_init(textmode_device, FILE_TYPE_DEVICE);

    textmode_device->write = (FsNodeWriteCallback)textmode_write;
    textmode_device->call = (FsNodeCallCallback)textmode_iocall;

    Path *textmode_device_path = path_create(TEXTMODE_DEVICE_PATH);
    filesystem_link_and_take_ref(textmode_device_path, textmode_device);
    path_destroy(textmode_device_path);
}
