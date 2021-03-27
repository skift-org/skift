#include <libwidget/Theme.h>

#include "terminal/Common.h"

RefPtr<Graphic::Font> font()
{
    static RefPtr<Graphic::Font> font = nullptr;

    if (font == nullptr)
    {
        font = Graphic::Font::get("mono").take_value();
    }

    return font;
}

static ThemeColorRole _color_to_role[Terminal::_COLOR_COUNT] = {
    [Terminal::BLACK] = THEME_ANSI_BLACK,
    [Terminal::RED] = THEME_ANSI_RED,
    [Terminal::GREEN] = THEME_ANSI_GREEN,
    [Terminal::YELLOW] = THEME_ANSI_YELLOW,
    [Terminal::BLUE] = THEME_ANSI_BLUE,
    [Terminal::MAGENTA] = THEME_ANSI_MAGENTA,
    [Terminal::CYAN] = THEME_ANSI_CYAN,
    [Terminal::GREY] = THEME_ANSI_WHITE,
    [Terminal::BRIGHT_BLACK] = THEME_ANSI_BRIGHT_BLACK,
    [Terminal::BRIGHT_RED] = THEME_ANSI_BRIGHT_RED,
    [Terminal::BRIGHT_GREEN] = THEME_ANSI_BRIGHT_GREEN,
    [Terminal::BRIGHT_YELLOW] = THEME_ANSI_BRIGHT_YELLOW,
    [Terminal::BRIGHT_BLUE] = THEME_ANSI_BRIGHT_BLUE,
    [Terminal::BRIGHT_MAGENTA] = THEME_ANSI_BRIGHT_MAGENTA,
    [Terminal::BRIGHT_CYAN] = THEME_ANSI_BRIGHT_CYAN,
    [Terminal::BRIGHT_GREY] = THEME_ANSI_BRIGHT_WHITE,
    [Terminal::FOREGROUND] = THEME_ANSI_FOREGROUND,
    [Terminal::BACKGROUND] = THEME_ANSI_BACKGROUND,
};

Graphic::Color color(Terminal::Color terminal_color)
{
    return theme_get_color(_color_to_role[terminal_color]);
}

static Vec2i _cell_size = Vec2i(7, 16);

Recti cell_bound(int x, int y)
{
    return {
        Vec2i(x, y) * _cell_size,
        _cell_size,
    };
}

Vec2i cell_size()
{
    return _cell_size;
}

void render_cell(
    Graphic::Painter &painter,
    int x,
    int y,
    Codepoint codepoint,
    Terminal::Color foreground,
    Terminal::Color background,
    Terminal::Attributes attributes)
{
    Recti bound = cell_bound(x, y);

    if (attributes.invert)
    {
        swap(foreground, background);
    }

    if (background != Terminal::BACKGROUND)
    {
        painter.clear(bound, color(background));
    }

    if (attributes.underline)
    {
        painter.draw_line(
            bound.position() + Vec2i(0, 14),
            bound.position() + Vec2i(bound.width(), 14),
            color(foreground));
    }

    if (codepoint == U' ')
    {
        return;
    }

    auto &glyph = font()->glyph(codepoint);

    painter.draw_glyph(
        *font(),
        glyph,
        bound.position() + Vec2i(0, 13),
        color(foreground));

    if (attributes.bold)
    {
        painter.draw_glyph(
            *font(),
            glyph,
            bound.position() + Vec2i(1, 13),
            color(foreground));
    }
}

void render_cell(Graphic::Painter &painter, int x, int y, Terminal::Cell cell)
{
    render_cell(painter, x, y, cell.codepoint, cell.attributes.foreground, cell.attributes.background, cell.attributes);
}
