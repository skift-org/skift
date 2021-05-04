#include "media-player/widgets/Cover.h"

#include <libgraphic/Painter.h>

namespace MediaPlayer
{

Cover::Cover(RefPtr<Graphic::Bitmap> bitmap) : _cover(bitmap)
{
    _backdrop = Graphic::Bitmap::create_shared(64, 64).unwrap();

    Graphic::Painter painter{*_backdrop};

    painter.blit(*_cover, _cover->bound(), _cover->bound().cover(_backdrop->bound()));
    painter.saturation(_backdrop->bound(), 1);
    painter.blur(_backdrop->bound(), 4);

    painter.fill_rectangle(_backdrop->bound(), Graphic::Colors::BLACK.with_alpha(0.5));
}

void Cover::paint(Graphic::Painter &painter, const Math::Recti &)
{
    painter.blit(*_backdrop, _backdrop->bound(), _backdrop->bound().cover(bound()));

    auto cover_bound = Math::Recti{0, 0, 256, 256}.centered_within(bound());

    painter.blit_rounded(*_cover, _cover->bound(), cover_bound, 12);
    painter.draw_rectangle_rounded(cover_bound, 12, 1, Graphic::Colors::WHITE.with_alpha(0.25));
}

} // namespace MediaPlayer
