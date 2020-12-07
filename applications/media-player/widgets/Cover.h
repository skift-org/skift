#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>
#include <libwidget/Widget.h>

namespace media_player
{

class Cover : public Widget
{
private:
    RefPtr<Bitmap> _bitmap;

public:
    Cover(Widget *parent, RefPtr<Bitmap> bitmap)
        : Widget(parent), _bitmap(bitmap)
    {
    }

    void paint(Painter &painter, Recti) override
    {
        painter.blit_bitmap_no_alpha(*_bitmap, _bitmap->bound(), _bitmap->bound().cover(bound()));

        painter.fill_rectangle(bound(), Colors::BLACK.with_alpha(0.5));
        painter.fill_rectangle(Recti{0, 0, 256, 256}.centered_within(bound()).offset({0, 8}), Colors::BLACK.with_alpha(0.5));
        painter.blur_rectangle(bound(), 16);

        painter.blit_bitmap_no_alpha(*_bitmap, _bitmap->bound(), Recti{0, 0, 256, 256}.centered_within(bound()));
        painter.draw_rectangle(Recti{0, 0, 256, 256}.centered_within(bound()), Colors::WHITE.with_alpha(0.25));
    }
};

} // namespace media_player
