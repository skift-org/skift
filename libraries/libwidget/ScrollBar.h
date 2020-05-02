#pragma once

#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;

    Point mouse_origine;

    int track;
    int thumb;
    int value;
} ScrollBar;

Widget *scrollbar_create(Widget *parent);
