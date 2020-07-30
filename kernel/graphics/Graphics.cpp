#include <libsystem/Logger.h>

#include "kernel/graphics/Graphics.h"

static bool _has_framebuffer;

void graphic_initialize(Multiboot *multiboot)
{
    if (_has_framebuffer)
    {
        return;
    }

    if (multiboot->framebuffer_pixelformat == PIXELFORMAT_CGA)
    {
        textmode_initialize();
    }
    else if (multiboot->framebuffer_pixelformat == PIXELFORMAT_RGB)
    {
        framebuffer_initialize(multiboot);
    }
    else
    {
        logger_error("No valid pixel format found!");
    }
}

void graphic_did_find_framebuffer()
{
    _has_framebuffer = true;
}
