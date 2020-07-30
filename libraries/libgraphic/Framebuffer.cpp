
#include <abi/IOCall.h>
#include <abi/Paths.h>

#include <libgraphic/Framebuffer.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/core/Plugs.h>

ResultOr<OwnPtr<Framebuffer>> Framebuffer::open(void)
{
    Handle handle;
    __plug_handle_open(&handle, FRAMEBUFFER_DEVICE_PATH, OPEN_READ | OPEN_WRITE);

    if (handle_has_error(&handle))
    {
        return handle_get_error(&handle);
    }

    IOCallDisplayModeArgs mode_info = {};
    __plug_handle_call(&handle, IOCALL_DISPLAY_GET_MODE, &mode_info);

    if (handle_has_error(&handle))
    {
        return handle_get_error(&handle);
    }

    auto bitmap_or_error = Bitmap::create_shared(mode_info.width, mode_info.height);

    if (!bitmap_or_error.success())
    {
        return bitmap_or_error.result();
    }

    return own<Framebuffer>(handle, bitmap_or_error.take_value());
}

Framebuffer::Framebuffer(Handle handle, RefPtr<Bitmap> bitmap)
    : _handle(handle),
      _bitmap(bitmap),
      _painter(bitmap)
{
}

Framebuffer::~Framebuffer()
{
    __plug_handle_close(&_handle);
}

Result Framebuffer::set_resolution(Vec2i size)
{
    auto bitmap_or_result = Bitmap::create_shared(size.x(), size.y());

    if (!bitmap_or_result.success())
    {
        return bitmap_or_result.result();
    }

    IOCallDisplayModeArgs mode_info = (IOCallDisplayModeArgs){size.x(), size.y()};

    __plug_handle_call(&_handle, IOCALL_DISPLAY_SET_MODE, &mode_info);

    if (handle_has_error(&_handle))
    {
        return handle_get_error(&_handle);
    }

    _bitmap = bitmap_or_result.take_value();
    _painter = Painter(_bitmap);

    return SUCCESS;
}

void Framebuffer::mark_dirty(Rectangle bound)
{
    bound = _bitmap->bound().clipped_with(bound);

    if (bound.is_empty())
    {
        return;
    }

    if (_bitmap->bound().colide_with(bound))
    {
        if (_is_dirty)
        {
            _dirty_bound = _dirty_bound.merged_with(bound);
        }
        else
        {
            _is_dirty = true;
            _dirty_bound = _bitmap->bound().clipped_with(bound);
        }
    }
}

void Framebuffer::mark_dirty_all()
{
    _is_dirty = true;
    _dirty_bound = _bitmap->bound();
}

void Framebuffer::blit()
{
    if (_dirty_bound.is_empty())
    {
        return;
    }

    IOCallDisplayBlitArgs args;

    args.buffer = (uint32_t *)_bitmap->pixels();
    args.buffer_width = _bitmap->width();
    args.buffer_height = _bitmap->height();

    args.blit_x = _dirty_bound.x();
    args.blit_y = _dirty_bound.y();
    args.blit_width = _dirty_bound.width();
    args.blit_height = _dirty_bound.height();

    __plug_handle_call(&_handle, IOCALL_DISPLAY_BLIT, &args);

    if (handle_has_error(&_handle))
    {
        handle_printf_error(&_handle, "Failled to iocall device " FRAMEBUFFER_DEVICE_PATH);
    }

    _is_dirty = false;
}
