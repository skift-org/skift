#pragma once

#include <libgraphic/Painter.h>
#include <libterminal/Cell.h>

RefPtr<Font> font();

Recti cell_bound(int x, int y);

Vec2i cell_size();

void render_cell(
    Painter &painter,
    int x,
    int y,
    Codepoint codepoint,
    terminal::Color foreground,
    terminal::Color background,
    terminal::Attributes attributes);

void render_cell(Painter &painter, int x, int y, terminal::Cell cell);
