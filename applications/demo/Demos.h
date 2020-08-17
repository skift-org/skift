#pragma once

#include <libsystem/eventloop/Timer.h>
#include <libwidget/Application.h>
#include <libwidget/Widget.h>

void fonts_draw(Painter &painter, Rectangle screen, float time);

void colors_draw(Painter &painter, Rectangle screen, float time);

void latency_draw(Painter &painter, Rectangle screen, float time);

void graphics_draw(Painter &painter, Rectangle screen, float time);

void lines_draw(Painter &painter, Rectangle screen, float time);

typedef void (*DrawDemoCallback)(Painter &painter, Rectangle screen, float time);

struct Demo
{
    const char *name;
    DrawDemoCallback callback;
} ;
