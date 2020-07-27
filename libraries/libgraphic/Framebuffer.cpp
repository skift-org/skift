
#include <abi/IOCall.h>
#include <abi/Paths.h>

#include <libgraphic/Framebuffer.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/core/Plugs.h>

Framebuffer *framebuffer_open(void)
{
    Framebuffer *framebuffer = __create(Framebuffer);

    __plug_handle_open(&framebuffer->handle, FRAMEBUFFER_DEVICE_PATH, OPEN_READ | OPEN_WRITE);

    if (handle_has_error(framebuffer))
    {
        return framebuffer;
    }

    IOCallDisplayModeArgs mode_info = {};

    __plug_handle_call(&framebuffer->handle, IOCALL_DISPLAY_GET_MODE, &mode_info);

    if (handle_has_error(framebuffer))
    {
        handle_printf_error(framebuffer, "Failled to do iocall on " FRAMEBUFFER_DEVICE_PATH);
        return framebuffer;
    }

    Bitmap *framebuffer_backbuffer = bitmap_create(mode_info.width, mode_info.height);

    framebuffer->backbuffer = framebuffer_backbuffer;
    framebuffer->painter = Painter(framebuffer_backbuffer);
    framebuffer->is_dirty = false;

    framebuffer->width = mode_info.width;
    framebuffer->height = mode_info.height;

    logger_info("Framebuffer create with graphic mode %dx%d.", mode_info.width, mode_info.height);

    return framebuffer;
}

Result framebuffer_set_mode(Framebuffer *framebuffer, int width, int height)
{
    IOCallDisplayModeArgs mode_info = (IOCallDisplayModeArgs){width, height};

    __plug_handle_call(&framebuffer->handle, IOCALL_DISPLAY_SET_MODE, &mode_info);

    if (handle_has_error(framebuffer))
    {
        handle_printf_error(framebuffer, "Failled to iocall device " FRAMEBUFFER_DEVICE_PATH);
        return handle_get_error(framebuffer);
    }

    bitmap_destroy(framebuffer->backbuffer);

    framebuffer->backbuffer = bitmap_create(width, height);
    framebuffer->painter = Painter(framebuffer->backbuffer);

    framebuffer->width = mode_info.width;
    framebuffer->height = mode_info.height;

    framebuffer_mark_dirty(framebuffer, bitmap_bound(framebuffer->backbuffer));

    return SUCCESS;
}

Rectangle framebuffer_bound(Framebuffer *framebuffer)
{
    return Rectangle(framebuffer->width, framebuffer->height);
}

void framebuffer_mark_dirty(Framebuffer *framebuffer, Rectangle bound)
{
    if (bitmap_bound(framebuffer->backbuffer).colide_with(bound))
    {
        bound = bitmap_bound(framebuffer->backbuffer).clipped_with(bound);

        if (framebuffer->is_dirty)
        {
            framebuffer->dirty_bound = framebuffer->dirty_bound.merged_with(bound);
        }
        else
        {
            framebuffer->is_dirty = true;
            framebuffer->dirty_bound = bitmap_bound(framebuffer->backbuffer).clipped_with(bound);
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
    if (bound.is_empty())
    {
        return;
    }

    IOCallDisplayBlitArgs args;

    args.buffer = (uint32_t *)framebuffer->backbuffer->pixels;
    args.buffer_width = bitmap_bound(framebuffer->backbuffer).width();
    args.buffer_height = bitmap_bound(framebuffer->backbuffer).height();

    args.blit_x = bound.x();
    args.blit_y = bound.y();
    args.blit_width = bound.width();
    args.blit_height = bound.height();

    __plug_handle_call(HANDLE(framebuffer), IOCALL_DISPLAY_BLIT, &args);

    if (handle_has_error(HANDLE(framebuffer)))
    {
        handle_printf_error(HANDLE(framebuffer), "Failled to iocall device " FRAMEBUFFER_DEVICE_PATH);
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

    if (framebuffer->backbuffer)
    {
        bitmap_destroy(framebuffer->backbuffer);
    }

    free(framebuffer);
}
