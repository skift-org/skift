#include <libsystem/Logger.h>

#include "kernel/graphics/Graphics.h"

static bool _has_framebuffer;

void graphic_initialize(Handover *handover)
{
    if (_has_framebuffer)
    {
        return;
    }

    framebuffer_initialize(handover);
}

void graphic_did_find_framebuffer()
{
    _has_framebuffer = true;
}
