#pragma once

#include <libgraphic/Painter.h>

void colors_draw(Graphic::Painter &painter, Math::Recti screen, float time);

void graphics_draw(Graphic::Painter &painter, Math::Recti screen, float time);

void lines_draw(Graphic::Painter &painter, Math::Recti screen, float time);

void path_draw(Graphic::Painter &Painter, Math::Recti screen, float time);

using DemoCallback = void (*)(Graphic::Painter &painter, Math::Recti screen, float time);

struct Demo
{
    const char *name;
    DemoCallback callback;
};
