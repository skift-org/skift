#include <libwidget/Theme.h>

#include "terminal/Common.h"

namespace terminal
{
    RefPtr<Font> font()
    {
        static RefPtr<Font> font = nullptr;

        if (font == nullptr)
        {
            font = Font::create("mono").take_value();
        }

        return font;
    }

    static ThemeColorRole _color_to_role[__TERMINAL_COLOR_COUNT] = {
        [TERMINAL_COLOR_BLACK] = THEME_ANSI_BLACK,
        [TERMINAL_COLOR_RED] = THEME_ANSI_RED,
        [TERMINAL_COLOR_GREEN] = THEME_ANSI_GREEN,
        [TERMINAL_COLOR_YELLOW] = THEME_ANSI_YELLOW,
        [TERMINAL_COLOR_BLUE] = THEME_ANSI_BLUE,
        [TERMINAL_COLOR_MAGENTA] = THEME_ANSI_MAGENTA,
        [TERMINAL_COLOR_CYAN] = THEME_ANSI_CYAN,
        [TERMINAL_COLOR_GREY] = THEME_ANSI_WHITE,
        [TERMINAL_COLOR_BRIGHT_BLACK] = THEME_ANSI_BRIGHT_BLACK,
        [TERMINAL_COLOR_BRIGHT_RED] = THEME_ANSI_BRIGHT_RED,
        [TERMINAL_COLOR_BRIGHT_GREEN] = THEME_ANSI_BRIGHT_GREEN,
        [TERMINAL_COLOR_BRIGHT_YELLOW] = THEME_ANSI_BRIGHT_YELLOW,
        [TERMINAL_COLOR_BRIGHT_BLUE] = THEME_ANSI_BRIGHT_BLUE,
        [TERMINAL_COLOR_BRIGHT_MAGENTA] = THEME_ANSI_BRIGHT_MAGENTA,
        [TERMINAL_COLOR_BRIGHT_CYAN] = THEME_ANSI_BRIGHT_CYAN,
        [TERMINAL_COLOR_BRIGHT_GREY] = THEME_ANSI_BRIGHT_WHITE,
        [TERMINAL_COLOR_DEFAULT_FOREGROUND] = THEME_ANSI_FOREGROUND,
        [TERMINAL_COLOR_DEFAULT_BACKGROUND] = THEME_ANSI_BACKGROUND,
    };

    Color color(TerminalColor terminal_color)
    {
        return theme_get_color(_color_to_role[terminal_color]);
    }

    static Vec2i _cell_size = Vec2i(7, 16);

    Rectangle cell_bound(int x, int y)
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
        TerminalColor foreground,
        TerminalColor background,
        TerminalAttributes attributes)
    {
        Rectangle bound = cell_bound(x, y);

        if (attributes.inverted)
        {
            swap(foreground, background);
        }

        painter.clear_rectangle(bound, color(background));

        if (attributes.underline)
        {
            painter.draw_line(
                bound.position() + Vec2i(0, 13),
                bound.position() + Vec2i(bound.width(), 13),
                color(foreground));
        }

        if (codepoint == U' ')
        {
            return;
        }

        Glyph &glyph = terminal::font()->glyph(codepoint);

        painter.draw_glyph(
            *terminal::font(),
            glyph,
            bound.position() + Vec2i(0, 12),
            color(foreground));

        if (attributes.bold)
        {
            painter.draw_glyph(
                *terminal::font(),
                glyph,
                bound.position() + Vec2i(1, 12),
                color(foreground));
        }
    }

    void render_cell(Painter &painter, int x, int y, TerminalCell cell)
    {
        render_cell(painter, x, y, cell.codepoint, cell.attributes.foreground, cell.attributes.background, cell.attributes);
    }
} // namespace terminal
