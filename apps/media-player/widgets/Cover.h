#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>
#include <libwidget/Widget.h>

namespace media_player
{

class Cover : public Widget
{
private:
    RefPtr<Bitmap> _cover;
    RefPtr<Bitmap> _backdrop;

public:
    Cover(Widget *parent, RefPtr<Bitmap> bitmap)
        : Widget(parent), _cover(bitmap)
    {
        _backdrop = Bitmap::create_shared(64, 64).value();

        Painter painter(_backdrop);

        painter.blit(*_cover, _cover->bound(), _cover->bound().cover(_backdrop->bound()));
        painter.saturation(_backdrop->bound(), 1);
        painter.blur(_backdrop->bound(), 4);

        painter.fill_rectangle(_backdrop->bound(), Colors::BLACK.with_alpha(0.5));
    }

    void paint(Painter &painter, Recti) override
    {
        painter.blit_no_alpha(*_backdrop, _backdrop->bound(), _backdrop->bound().cover(bound()));

        painter.blit_no_alpha(*_cover, _cover->bound(), Recti{0, 0, 256, 256}.centered_within(bound()));
        painter.draw_rectangle(Recti{0, 0, 256, 256}.centered_within(bound()), Colors::WHITE.with_alpha(0.25));
    }
};

} // namespace media_player
