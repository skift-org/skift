#pragma once

#include <libgraphic/bitmap.h>
#include <libgraphic/painter.h>
#include <libsystem/runtime.h>
#include <libwidget/core/Widget.h>

typedef struct
{
    Widget widget;

    int id;

    int framebuffer_handle;
    Bitmap *framebuffer;
    Painter *painter;
} Window;

Widget *window_create(Rectangle bound);

void window_destroy(Widget *window);
