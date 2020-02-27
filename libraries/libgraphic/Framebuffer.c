/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <abi/IOCall.h>

#include <libgraphic/Framebuffer.h>
#include <libsystem/Result.h>
#include <libsystem/__plugs__.h>
#include <libsystem/logger.h>

Framebuffer *framebuffer_open(void)
{
    Framebuffer *framebuffer = __create(Framebuffer);

    __plug_handle_open(&framebuffer->handle, "/dev/framebuffer", OPEN_READ | OPEN_WRITE);

    if (handle_has_error(framebuffer))
    {
        return framebuffer;
    }

    IOCallDisplayModeArgs mode_info = {0};

    __plug_handle_call(&framebuffer->handle, IOCALL_DISPLAY_GET_MODE, &mode_info);

    if (handle_has_error(framebuffer))
    {
        handle_printf_error(framebuffer, "Failled to do iocall on /dev/framebuffer");
        return framebuffer;
    }

    Bitmap *framebuffer_backbuffer = bitmap_create(mode_info.width, mode_info.height);

    Painter *framebuffer_painter = painter_create(framebuffer_backbuffer);

    framebuffer->backbuffer = framebuffer_backbuffer;
    framebuffer->painter = framebuffer_painter;
    framebuffer->is_dirty = false;

    framebuffer->width = mode_info.width;
    framebuffer->height = mode_info.height;

    logger_info("Framebuffer create with graphic mode %dx%d.", mode_info.width, mode_info.height);

    return framebuffer;
}

Result framebuffer_set_mode(Framebuffer *framebuffer, int width, int height)
{
    IOCallDisplayModeArgs mode_info = (IOCallDisplayModeArgs){width, height};

    __plug_handle_call(&framebuffer->handle, IOCALL_DISPLAY_GET_MODE, &mode_info);

    if (handle_has_error(framebuffer))
    {
        handle_printf_error(framebuffer, "Failled to iocall device /dev/framebuffer");
        return handle_get_error(framebuffer);
    }

    painter_destroy(framebuffer->painter);
    bitmap_destroy(framebuffer->backbuffer);

    framebuffer->backbuffer = bitmap_create(width, height);
    framebuffer->painter = painter_create(framebuffer->backbuffer);

    framebuffer->width = mode_info.width;
    framebuffer->height = mode_info.height;

    framebuffer_mark_dirty(framebuffer, bitmap_bound(framebuffer->backbuffer));

    return true;
}

Rectangle framebuffer_bound(Framebuffer *framebuffer)
{
    return (Rectangle){{0, 0, framebuffer->width, framebuffer->height}};
}

void framebuffer_mark_dirty(Framebuffer *framebuffer, Rectangle bound)
{
    if (rectangle_colide(bitmap_bound(framebuffer->backbuffer), bound))
    {
        bound = rectangle_clip(bitmap_bound(framebuffer->backbuffer), bound);

        if (framebuffer->is_dirty)
        {
            framebuffer->dirty_bound = rectangle_merge(framebuffer->dirty_bound, bound);
        }
        else
        {
            framebuffer->is_dirty = true;
            framebuffer->dirty_bound = rectangle_clip(bitmap_bound(framebuffer->backbuffer), bound);
        }
    }
}

void framebuffer_mark_dirty_all(Framebuffer *framebuffer)
{
    framebuffer->is_dirty = true;
    framebuffer->dirty_bound = bitmap_bound(framebuffer->backbuffer);
}

void framebuffer_blit_region(Framebuffer *framebuffer, Rectangle bound)
{
    if (rectangle_is_empty(bound))
    {
        return;
    }

    IOCallDisplayBlitArgs args;

    args.buffer = framebuffer->backbuffer->pixels;
    args.buffer_width = bitmap_bound(framebuffer->backbuffer).size.X;
    args.buffer_height = bitmap_bound(framebuffer->backbuffer).size.Y;

    args.blit_x = bound.X;
    args.blit_y = bound.Y;
    args.blit_width = bound.width;
    args.blit_height = bound.height;

    __plug_handle_call(HANDLE(framebuffer), IOCALL_DISPLAY_BLIT, &args);

    if (handle_has_error(HANDLE(framebuffer)))
    {
        handle_printf_error(HANDLE(framebuffer), "Failled to iocall device /dev/framebuffer");
    }
}

void framebuffer_blit_dirty(Framebuffer *framebuffer)
{
    if (framebuffer->is_dirty)
    {
        framebuffer_blit_region(framebuffer, framebuffer->dirty_bound);

        framebuffer->is_dirty = false;
    }
}

void framebuffer_blit(Framebuffer *framebuffer)
{
    framebuffer_blit_region(framebuffer, bitmap_bound(framebuffer->backbuffer));
    framebuffer->is_dirty = false;
}

void framebuffer_close(Framebuffer *framebuffer)
{
    __plug_handle_close(HANDLE(framebuffer));

    if (framebuffer->painter)
    {
        painter_destroy(framebuffer->painter);
    }

    if (framebuffer->backbuffer)
    {
        bitmap_destroy(framebuffer->backbuffer);
    }

    free(framebuffer);
}