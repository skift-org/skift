#include <libsystem/Logger.h>

#include "kernel/graphics/Graphics.h"
#include "kernel/interrupts/Interupts.h"

static uintptr_t _framebuffer_address = 0;
static int _framebuffer_width = 0;
static int _framebuffer_height = 0;
static int _framebuffer_pitch = 0;
static int _framebuffer_bpp = 0;

void graphic_early_initialize(Handover *handover)
{
    graphic_did_find_framebuffer(
        handover->framebuffer_addr,
        handover->framebuffer_width,
        handover->framebuffer_height,
        handover->framebuffer_pitch,
        handover->framebuffer_bpp);
}

void graphic_initialize(Handover *handover)
{
    if (_framebuffer_address != 0)
    {
        return;
    }

    framebuffer_initialize(handover);
}

void graphic_did_find_framebuffer(uintptr_t address, int width, int height, int pitch, int bpp)
{
    InterruptsRetainer retainer;

    _framebuffer_address = address;
    _framebuffer_width = width;
    _framebuffer_height = height;
    _framebuffer_pitch = pitch;
    _framebuffer_bpp = bpp;

    logger_info("Framebuffer: w:%d h:%d p:%d bpp:%d", width, height, pitch, bpp);
}

bool graphic_has_framebuffer()
{
    return _framebuffer_address != 0;
}

uint32_t *graphic_framebuffer()
{
    return reinterpret_cast<uint32_t *>(_framebuffer_address);
}

int graphic_framebuffer_width()
{
    return _framebuffer_width;
}

int graphic_framebuffer_height()
{
    return _framebuffer_height;
}

int graphic_framebuffer_pitch()
{
    return _framebuffer_pitch;
}

void graphic_framebuffer_plot(int x, int y, uint32_t color)
{
    if ((x >= 0 && x <= graphic_framebuffer_width()) &&
        (y >= 0 && y <= graphic_framebuffer_height()))
    {
        uint8_t *pixel = (reinterpret_cast<uint8_t *>(_framebuffer_address)) + (y * (_framebuffer_pitch)) + (x * _framebuffer_bpp);

        if (_framebuffer_bpp == 4)
        {
            pixel[0] = (color >> 0) & 0xff;
            pixel[1] = (color >> 8) & 0xff;
            pixel[2] = (color >> 16) & 0xff;
            pixel[4] = 0x00;
        }
        else if (_framebuffer_bpp == 3)
        {
            pixel[0] = color & 0xff;
            pixel[1] = (color >> 8) & 0xff;
            pixel[2] = (color >> 16) & 0xff;
        }
    }
}