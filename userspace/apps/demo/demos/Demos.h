#pragma once

#include <libasync/Timer.h>
#include <libwidget/Application.h>
#include <libwidget/Component.h>

void colors_draw(Graphic::Painter &painter, Math::Recti screen, float time);

void graphics_draw(Graphic::Painter &painter, Math::Recti screen, float time);

void lines_draw(Graphic::Painter &painter, Math::Recti screen, float time);

void path_draw(Graphic::Painter &Painter, Math::Recti screen, float time);

typedef void (*DrawDemoCallback)(Graphic::Painter &painter, Math::Recti screen, float time);

struct Demo
{
    const char *name;
    DrawDemoCallback callback;
};
