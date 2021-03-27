#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>
#include <libsystem/io/Handle.h>
#include <libutils/OwnPtr.h>

namespace Graphic
{

class Framebuffer
{
private:
    Handle _handle;

    RefPtr<Bitmap> _bitmap;
    Painter _painter;

    Vector<Recti> _dirty_bounds{};

public:
    static ResultOr<OwnPtr<Framebuffer>> open();

    Painter &painter() { return _painter; }

    Recti resolution() { return _bitmap->bound(); }

    Framebuffer(Handle handle, RefPtr<Bitmap> bitmap);

    ~Framebuffer();

    Result set_resolution(Vec2i size);

    void mark_dirty(Recti rectangle);

    void mark_dirty_all();

    void blit();
};

} // namespace Graphic
