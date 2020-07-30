#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>
#include <libsystem/io/Handle.h>
#include <libutils/OwnPtr.h>

class Framebuffer
{
private:
    Handle _handle;

    RefPtr<Bitmap> _bitmap;
    Painter _painter;

    bool _is_dirty = false;
    Rectangle _dirty_bound = Rectangle::empty();

public:
    Painter &painter() { return _painter; }

    Rectangle resolution() { return _bitmap->bound(); }

    static ResultOr<OwnPtr<Framebuffer>> open();

    Framebuffer(Handle handle, RefPtr<Bitmap> bitmap);

    ~Framebuffer();

    Result set_resolution(Vec2i size);

    void mark_dirty(Rectangle rectangle);

    void mark_dirty_all();

    void blit();
};
