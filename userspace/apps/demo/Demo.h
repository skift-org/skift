#pragma once

#include <libgraphic/Painter.h>

namespace Demo
{

void colors(Graphic::Painter &painter, Math::Recti screen, float time);

void graphics(Graphic::Painter &painter, Math::Recti screen, float time);

void lines(Graphic::Painter &painter, Math::Recti screen, float time);

void path(Graphic::Painter &Painter, Math::Recti screen, float time);

using DemoCallback = void (*)(Graphic::Painter &painter, Math::Recti screen, float time);

struct Demo
{
    const char *name;
    DemoCallback callback;
};

} // namespace Demo