#pragma once

#include <libgraphic/Painter.h>
#include <libutils/Callback.h>

class PaintDocument : public RefCounted<PaintDocument>
{
private:
    RefPtr<Graphic::Bitmap> _bitmap;
    Graphic::Painter _painter;

    Graphic::Color _primary_color = Graphic::Colors::BLACK;
    Graphic::Color _secondary_color = Graphic::Colors::WHITE;

    bool _dirty = true;

public:
    Callback<void()> on_color_change;

    Recti bound() { return _bitmap->bound(); }
    Graphic::Bitmap &bitmap() { return *_bitmap; }
    Graphic::Painter &painter() { return _painter; }

    bool dirty() { return _dirty; }
    void dirty(bool value) { _dirty = value; }

    Graphic::Color primary_color() { return _primary_color; }

    void primary_color(Graphic::Color value)
    {
        _primary_color = value;

        if (on_color_change)
            on_color_change();
    }

    Graphic::Color secondary_color()
    {

        return _secondary_color;
    }

    void secondary_color(Graphic::Color value)
    {
        _secondary_color = value;

        if (on_color_change)
            on_color_change();
    }

    PaintDocument(RefPtr<Graphic::Bitmap> bitmap)
        : _bitmap(bitmap),
          _painter(bitmap)
    {
    }
};
