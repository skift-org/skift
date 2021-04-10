#pragma once

#include <libgraphic/Painter.h>
#include <libterminal/Cell.h>

RefPtr<Graphic::Font> font();

Math::Recti cell_bound(int x, int y);

Math::Vec2i cell_size();

void render_cell(
    Graphic::Painter &painter,
    int x,
    int y,
    Codepoint codepoint,
    Terminal::Color foreground,
    Terminal::Color background,
    Terminal::Attributes attributes);

void render_cell(Graphic::Painter &painter, int x, int y, Terminal::Cell cell);
