#include <libgraphic/TrueTypeFont.h>

#include "demo/Demos.h"

static TrueTypeFamily *_family = NULL;
static TrueTypeFont *_fonts[16];

void fonts_draw(Painter *painter, Rectangle screen, double time)
{
    __unused(screen);
    __unused(time);

    if (!_family)
    {
        _family = truetype_family_create("/res/fonts/RobotoMono/RobotoMono-Medium.ttf");

        for (size_t i = 0; i < 16; i++)
        {
            _fonts[i] = truetypefont_create(_family, 8 + 4 * i);
        }
    }

    painter_clear(painter, COLOR_REBECCAPURPLE);

    int current = 8;
    for (size_t i = 0; i < 16; i++)
    {
        painter_draw_truetype_string(painter, _fonts[i], "The quick brown fox jumps over the lazy dog.", vec2i(8, current), COLOR_WHITE);

        current += 8 + 4 * i;
    }
}
