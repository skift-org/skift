#pragma once

#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;

    Vec2i mouse_origine;

    int track;
    int thumb;
    int value;
} ScrollBar;

Widget *scrollbar_create(Widget *parent);
