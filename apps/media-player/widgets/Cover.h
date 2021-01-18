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
    RefPtr<Bitmap> _bitmap_scaled_and_blur;

public:
    Cover(Widget *parent, RefPtr<Bitmap> bitmap)
        : Widget(parent), _bitmap(bitmap)
    {
        _bitmap_scaled_and_blur = Bitmap::create_shared(64, 64).value();

        Painter painter(_bitmap_scaled_and_blur);
        painter.blit_bitmap(*_bitmap, _bitmap->bound(), _bitmap->bound().cover(_bitmap_scaled_and_blur->bound()));
        painter.saturation(_bitmap_scaled_and_blur->bound(), 1);
        painter.blur_rectangle(_bitmap_scaled_and_blur->bound(), 4);
        painter.fill_rectangle(_bitmap_scaled_and_blur->bound(), Colors::BLACK.with_alpha(0.5));
    }

    void paint(Painter &painter, Recti) override
    {
        painter.blit_bitmap_no_alpha(*_bitmap_scaled_and_blur, _bitmap_scaled_and_blur->bound(), _bitmap_scaled_and_blur->bound().cover(bound()));

        painter.blit_bitmap_no_alpha(*_bitmap, _bitmap->bound(), Recti{0, 0, 256, 256}.centered_within(bound()));
        painter.draw_rectangle(Recti{0, 0, 256, 256}.centered_within(bound()), Colors::WHITE.with_alpha(0.25));
    }
};

} // namespace media_player
