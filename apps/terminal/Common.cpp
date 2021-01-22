#include <libwidget/Theme.h>

#include "terminal/Common.h"

RefPtr<Font> font()
{
    static RefPtr<Font> font = nullptr;

    if (font == nullptr)
    {
        font = Font::get("mono").take_value();
    }

    return font;
}

static ThemeColorRole _color_to_role[terminal::_COLOR_COUNT] = {
    [terminal::BLACK] = THEME_ANSI_BLACK,
    [terminal::RED] = THEME_ANSI_RED,
    [terminal::GREEN] = THEME_ANSI_GREEN,
    [terminal::YELLOW] = THEME_ANSI_YELLOW,
    [terminal::BLUE] = THEME_ANSI_BLUE,
    [terminal::MAGENTA] = THEME_ANSI_MAGENTA,
    [terminal::CYAN] = THEME_ANSI_CYAN,
    [terminal::GREY] = THEME_ANSI_WHITE,
    [terminal::BRIGHT_BLACK] = THEME_ANSI_BRIGHT_BLACK,
    [terminal::BRIGHT_RED] = THEME_ANSI_BRIGHT_RED,
    [terminal::BRIGHT_GREEN] = THEME_ANSI_BRIGHT_GREEN,
    [terminal::BRIGHT_YELLOW] = THEME_ANSI_BRIGHT_YELLOW,
    [terminal::BRIGHT_BLUE] = THEME_ANSI_BRIGHT_BLUE,
    [terminal::BRIGHT_MAGENTA] = THEME_ANSI_BRIGHT_MAGENTA,
    [terminal::BRIGHT_CYAN] = THEME_ANSI_BRIGHT_CYAN,
    [terminal::BRIGHT_GREY] = THEME_ANSI_BRIGHT_WHITE,
    [terminal::FOREGROUND] = THEME_ANSI_FOREGROUND,
    [terminal::BACKGROUND] = THEME_ANSI_BACKGROUND,
};

Color color(terminal::Color terminal_color)
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
    Painter &painter,
    int x,
    int y,
    Codepoint codepoint,
    terminal::Color foreground,
    terminal::Color background,
    terminal::Attributes attributes)
{
    Recti bound = cell_bound(x, y);

    if (attributes.invert)
    {
        swap(foreground, background);
    }

    if (background != terminal::BACKGROUND)
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

void render_cell(Painter &painter, int x, int y, terminal::Cell cell)
{
    render_cell(painter, x, y, cell.codepoint, cell.attributes.foreground, cell.attributes.background, cell.attributes);
}
