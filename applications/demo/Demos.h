#pragma once

#include <libsystem/eventloop/Timer.h>
#include <libwidget/Application.h>
#include <libwidget/Widget.h>

void colors_draw(Painter *painter, Rectangle screen, double time);

void cube_draw(Painter *painter, Rectangle screen, double time);

void graphics_draw(Painter *painter, Rectangle screen, double time);

void lines_draw(Painter *painter, Rectangle screen, double time);

typedef void (*DrawDemoCallback)(Painter *painter, Rectangle screen, double time);

typedef struct
{
    const char *name;
    DrawDemoCallback callback;
} Demo;