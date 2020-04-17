#pragma once

#include <libwidget/Image.h>
#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;

    Bitmap *bitmap;
} Icon;

Widget *icon_create(Widget *parent, const char *path);