
#include <abi/IOCall.h>
#include <abi/Paths.h>

#include <libgraphic/Framebuffer.h>
#include <libsystem/Result.h>
#include <libsystem/core/Plugs.h>

namespace Graphic
{

ResultOr<OwnPtr<Framebuffer>> Framebuffer::open()
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

    return own<Framebuffer>(handle, bitmap_or_error.unwrap());
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
    auto bitmap = TRY(Bitmap::create_shared(size.x(), size.y()));

    IOCallDisplayModeArgs mode_info = (IOCallDisplayModeArgs){size.x(), size.y()};

    __plug_handle_call(&_handle, IOCALL_DISPLAY_SET_MODE, &mode_info);

    if (handle_has_error(&_handle))
    {
        return handle_get_error(&_handle);
    }

    _bitmap = bitmap;
    _painter = Painter(_bitmap);

    return SUCCESS;
}

void Framebuffer::mark_dirty(Recti new_bound)
{
    new_bound = _bitmap->bound().clipped_with(new_bound);

    if (new_bound.is_empty())
    {
        return;
    }

    bool merged = false;

    _dirty_bounds.foreach ([&](Recti &region) {
        int region_area = region.area();
        int merge_area = region.merged_with(new_bound).area();

        if (region.colide_with(new_bound) && (region_area + new_bound.area() > merge_area))
        {
            region = region.merged_with(new_bound);
            merged = true;

            return Iteration::STOP;
        }

        return Iteration::CONTINUE;
    });

    if (!merged)
    {
        _dirty_bounds.push_back(new_bound);
    }
}

void Framebuffer::mark_dirty_all()
{
    _dirty_bounds.clear();
    mark_dirty(_bitmap->bound());
}

void Framebuffer::blit()
{
    if (_dirty_bounds.empty())
    {
        return;
    }

    _dirty_bounds.foreach ([&](auto &bound) {
        IOCallDisplayBlitArgs args;

        args.buffer = reinterpret_cast<uint32_t *>(_bitmap->pixels());
        args.buffer_width = _bitmap->width();
        args.buffer_height = _bitmap->height();

        args.blit_x = bound.x();
        args.blit_y = bound.y();
        args.blit_width = bound.width();
        args.blit_height = bound.height();

        __plug_handle_call(&_handle, IOCALL_DISPLAY_BLIT, &args);

        if (handle_has_error(&_handle))
        {
            handle_printf_error(&_handle, "Failed to iocall device " FRAMEBUFFER_DEVICE_PATH);
        }

        return Iteration::CONTINUE;
    });

    _dirty_bounds.clear();
}

} // namespace Graphic
