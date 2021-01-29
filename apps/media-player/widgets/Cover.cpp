#include "media-player/widgets/Cover.h"

#include <libgraphic/Painter.h>

namespace media_player
{

Cover::Cover(Widget *parent, RefPtr<Bitmap> bitmap)
    : Widget(parent), _cover(bitmap)
{
    _backdrop = *Bitmap::create_shared(64, 64);

    Painter painter(_backdrop);

    painter.blit(*_cover, _cover->bound(), _cover->bound().cover(_backdrop->bound()));
    painter.saturation(_backdrop->bound(), 1);
    painter.blur(_backdrop->bound(), 4);

    painter.fill_rectangle(_backdrop->bound(), Colors::BLACK.with_alpha(0.5));
}

void Cover::paint(Painter &painter, const Recti &)
{
    painter.blit_no_alpha(*_backdrop, _backdrop->bound(), _backdrop->bound().cover(bound()));

    auto cover_bound = Recti{0, 0, 256, 256}.centered_within(bound());

    painter.blit_no_alpha(*_cover, _cover->bound(), cover_bound);
    painter.draw_rectangle(cover_bound, Colors::WHITE.with_alpha(0.25));
}

} // namespace media_player
