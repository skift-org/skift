#pragma once

#include <libutils/unicode/Codepoint.h>

int font_height();

int font_width();

void font_draw(Codepoint cp, int x, int y);

void font_set_fg(int color);
void font_set_bg(int color);
