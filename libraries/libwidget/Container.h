#pragma once

#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;
} Container;

Widget *container_create(Widget *parent, Rectangle bound);
