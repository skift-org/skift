#pragma once

#include <libgraphic/Painter.h>
#include <libutils/Callback.h>

class PaintDocument : public RefCounted<PaintDocument>
{
private:
    RefPtr<Bitmap> _bitmap;
    Painter _painter;

    Color _primary_color = Colors::BLACK;
    Color _secondary_color = Colors::WHITE;

    bool _dirty = true;

public:
    Callback<void()> on_color_change;

    Recti bound() { return _bitmap->bound(); }
    Bitmap &bitmap() { return *_bitmap; }
    Painter &painter() { return _painter; }

    bool dirty() { return _dirty; }
    void dirty(bool value) { _dirty = value; }

    Color primary_color() { return _primary_color; }

    void primary_color(Color value)
    {
        _primary_color = value;

        if (on_color_change)
            on_color_change();
    }

    Color secondary_color()
    {

        return _secondary_color;
    }

    void secondary_color(Color value)
    {
        _secondary_color = value;

        if (on_color_change)
            on_color_change();
    }

    PaintDocument(RefPtr<Bitmap> bitmap)
        : _bitmap(bitmap),
          _painter(bitmap)
    {
    }
};
