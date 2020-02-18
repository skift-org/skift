#pragma once

#include <libgraphic/Painter.h>

void style_draw_button(Painter *painter, Rectangle bound);
void style_draw_button_focus(Painter *painter, Rectangle bound);
void style_draw_button_over(Painter *painter, Rectangle bound);
void style_draw_button_down(Painter *painter, Rectangle bound);
