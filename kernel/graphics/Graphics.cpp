#include "kernel/graphics/Graphics.h"
#include "kernel/interrupts/Interupts.h"

static uintptr_t _framebuffer_address = 0;
static int _framebuffer_width = 0;
static int _framebuffer_height = 0;

void graphic_early_initialize(Handover *handover)
{
    _framebuffer_address = handover->framebuffer_addr;
    _framebuffer_width = handover->framebuffer_width;
    _framebuffer_height = handover->framebuffer_height;
}

void graphic_initialize(Handover *handover)
{
    if (_framebuffer_address != 0)
    {
        return;
    }

    framebuffer_initialize(handover);
}

void graphic_did_find_framebuffer(uintptr_t address, int width, int height)
{
    InterruptsRetainer retainer;

    _framebuffer_address = address;
    _framebuffer_width = width;
    _framebuffer_height = height;
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
