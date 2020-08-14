#pragma once

#include <libgraphic/Painter.h>
#include <libterminal/Cell.h>

namespace terminal
{
    RefPtr<Font> font();

    Rectangle cell_bound(int x, int y);

    Vec2i cell_size();

    void render_cell(
        Painter &painter,
        int x,
        int y,
        Codepoint codepoint,
        TerminalColor foreground,
        TerminalColor background,
        TerminalAttributes attributes);

    void render_cell(
        Painter &painter,
        int x,
        int y,
        TerminalCell cell);
} // namespace terminal
