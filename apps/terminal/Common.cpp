#include <libwidget/Theme.h>

#include "terminal/Common.h"

RefPtr<Graphic::Font> font()
{
    static RefPtr<Graphic::Font> font = nullptr;

    if (font == nullptr)
    {
        font = Graphic::Font::get("mono").unwrap();
    }

    return font;
}

static Widget::ThemeColorRole _color_to_role[Terminal::_COLOR_COUNT] = {
    [Terminal::BLACK] = Widget::THEME_ANSI_BLACK,
    [Terminal::RED] = Widget::THEME_ANSI_RED,
    [Terminal::GREEN] = Widget::THEME_ANSI_GREEN,
    [Terminal::YELLOW] = Widget::THEME_ANSI_YELLOW,
    [Terminal::BLUE] = Widget::THEME_ANSI_BLUE,
    [Terminal::MAGENTA] = Widget::THEME_ANSI_MAGENTA,
    [Terminal::CYAN] = Widget::THEME_ANSI_CYAN,
    [Terminal::GREY] = Widget::THEME_ANSI_WHITE,
    [Terminal::BRIGHT_BLACK] = Widget::THEME_ANSI_BRIGHT_BLACK,
    [Terminal::BRIGHT_RED] = Widget::THEME_ANSI_BRIGHT_RED,
    [Terminal::BRIGHT_GREEN] = Widget::THEME_ANSI_BRIGHT_GREEN,
    [Terminal::BRIGHT_YELLOW] = Widget::THEME_ANSI_BRIGHT_YELLOW,
    [Terminal::BRIGHT_BLUE] = Widget::THEME_ANSI_BRIGHT_BLUE,
    [Terminal::BRIGHT_MAGENTA] = Widget::THEME_ANSI_BRIGHT_MAGENTA,
    [Terminal::BRIGHT_CYAN] = Widget::THEME_ANSI_BRIGHT_CYAN,
    [Terminal::BRIGHT_GREY] = Widget::THEME_ANSI_BRIGHT_WHITE,
    [Terminal::FOREGROUND] = Widget::THEME_ANSI_FOREGROUND,
    [Terminal::BACKGROUND] = Widget::THEME_ANSI_BACKGROUND,
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
