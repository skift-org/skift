#include <libgraphic/TrueTypeFont.h>

#include "demo/Demos.h"

static TrueTypeFamily *_family = nullptr;
static TrueTypeFont *_fonts[16];

void fonts_draw(Painter &painter, Rectangle screen, float time)
{
    __unused(screen);
    __unused(time);

    if (!_family)
    {
        _family = truetype_family_create("/Files/Fonts/Roboto/Roboto-Medium.ttf");

        for (size_t i = 0; i < 16; i++)
        {
            _fonts[i] = truetypefont_create(_family, 8 + 4 * i);
        }
    }

    painter.clear(Colors::REBECCAPURPLE);

    int current = 4;
    for (size_t i = 0; i < 16; i++)
    {
        TrueTypeFontMetrics metrics = truetypefont_get_metrics(_fonts[i]);

        current += metrics.ascent;

        // FIXME: We should use char8_t
        painter.draw_truetype_string(
            _fonts[i],
            reinterpret_cast<const char *>(u8"The quick brown fox jumps over the lazy dog"),
            Vec2i(8, current),
            Colors::WHITE);

        current -= metrics.descent;
        current += metrics.linegap;
    }
}
