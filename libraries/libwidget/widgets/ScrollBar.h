#pragma once

#include <libwidget/Widget.h>

struct ScrollBar
{
    Widget widget;

    Vec2i mouse_origine;

    int track;
    int thumb;
    int value;
};

Widget *scrollbar_create(Widget *parent);
