/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libdevice/framebuffer.h>
#include <libgraphic/Framebuffer.h>
#include <libsystem/Result.h>
#include <libsystem/logger.h>

Framebuffer *framebuffer_open(void)
{
    Stream *framebuffer_device = stream_open(FRAMEBUFFER_DEVICE, OPEN_READ | OPEN_WRITE);

    if (handle_has_error(framebuffer_device))
    {
        stream_close(framebuffer_device);
        return NULL;
    }

    framebuffer_mode_arg_t mode_info;

    if (stream_call(framebuffer_device, FRAMEBUFFER_CALL_GET_MODE, &mode_info) < 0)
    {
        handle_printf_error(framebuffer_device, "Failled to iocall" FRAMEBUFFER_DEVICE);
        return NULL;
    }

    logger_info("Framebuffer create with graphic mode %dx%d.", mode_info.size.X, mode_info.size.Y);

    Bitmap *framebuffer_backbuffer = bitmap_create(mode_info.size.X, mode_info.size.Y);

    Painter *framebuffer_painter = painter_create(framebuffer_backbuffer);

    Framebuffer *framebuffer = __create(Framebuffer);

    framebuffer->device = framebuffer_device;
    framebuffer->backbuffer = framebuffer_backbuffer;
    framebuffer->painter = framebuffer_painter;
    framebuffer->is_dirty = false;

    framebuffer->width = mode_info.size.X;
    framebuffer->height = mode_info.size.Y;

    return framebuffer;
}

bool framebuffer_set_mode(Framebuffer *framebuffer, int resx, int resy)
{
    framebuffer_mode_arg_t mode_info = (framebuffer_mode_arg_t){
        .size = (Point){resx, resy},
    };

    if (stream_call(framebuffer->device, FRAMEBUFFER_CALL_SET_MODE, &mode_info) != SUCCESS)
    {
        handle_printf_error(framebuffer->device, "Failled to iocall device %s", FRAMEBUFFER_DEVICE);
        return false;
    }

    painter_destroy(framebuffer->painter);
    bitmap_destroy(framebuffer->backbuffer);

    framebuffer->backbuffer = bitmap_create(resx, resy);
    framebuffer->painter = painter_create(framebuffer->backbuffer);

    framebuffer->width = mode_info.size.X;
    framebuffer->height = mode_info.size.Y;

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

void framebuffer_blit_dirty(Framebuffer *framebuffer)
{
    if (framebuffer->is_dirty)
    {
        framebuffer_blitregion_args_t args;

        args.buffer = framebuffer->backbuffer->pixels;
        args.size = bitmap_bound(framebuffer->backbuffer).size;
        args.region_to_blit = framebuffer->dirty_bound;

        if (stream_call(framebuffer->device, FRAMEBUFFER_CALL_BLITREGION, &args) != SUCCESS)
        {
            handle_printf_error(framebuffer->device, "Failled to iocall device %s: %s", FRAMEBUFFER_DEVICE);
        }

        framebuffer->is_dirty = false;
    }
}

void framebuffer_blit(Framebuffer *framebuffer)
{
    framebuffer_blit_args_t args;

    args.buffer = framebuffer->backbuffer->pixels;
    args.size = bitmap_bound(framebuffer->backbuffer).size;

    if (stream_call(framebuffer->device, FRAMEBUFFER_CALL_BLIT, &args) != SUCCESS)
    {
        handle_printf_error(framebuffer->device, "Failled to iocall device %s:", FRAMEBUFFER_DEVICE);
    }

    framebuffer->is_dirty = false;
}

void framebuffer_close(Framebuffer *framebuffer)
{
    stream_close(framebuffer->device);
    painter_destroy(framebuffer->painter);
    bitmap_destroy(framebuffer->backbuffer);

    free(framebuffer);
}