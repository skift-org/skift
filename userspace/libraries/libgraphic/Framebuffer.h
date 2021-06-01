#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>
#include <libutils/OwnPtr.h>

namespace Graphic
{

struct Framebuffer
{
private:
    Handle _handle;

    RefPtr<Bitmap> _bitmap;

    Vector<Math::Recti> _dirty_bounds{};

public:
    static ResultOr<OwnPtr<Framebuffer>> open();

    Math::Recti resolution() { return _bitmap->bound(); }

    Bitmap &bitmap() { return *_bitmap; }

    Framebuffer(Handle handle, RefPtr<Bitmap> bitmap);

    ~Framebuffer();

    HjResult set_resolution(Math::Vec2i size);

    void mark_dirty(Math::Recti rectangle);

    void mark_dirty_all();

    void blit();
};

} // namespace Graphic
