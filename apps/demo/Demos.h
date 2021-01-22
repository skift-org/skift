#pragma once

#include <libsystem/eventloop/Timer.h>
#include <libwidget/Application.h>
#include <libwidget/Widget.h>

void colors_draw(Painter &painter, Recti screen, float time);

void graphics_draw(Painter &painter, Recti screen, float time);

void lines_draw(Painter &painter, Recti screen, float time);

void path_draw(Painter &Painter, Recti screen, float time);

typedef void (*DrawDemoCallback)(Painter &painter, Recti screen, float time);

struct Demo
{
    const char *name;
    DrawDemoCallback callback;
};
