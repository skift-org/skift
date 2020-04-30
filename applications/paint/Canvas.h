#pragma once

#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;

    Bitmap *bitmap;
    Painter *painter;
} Canvas;

Widget *canvas_create(Widget *parent, int width, int height);