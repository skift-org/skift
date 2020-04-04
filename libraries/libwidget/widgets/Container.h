#pragma once

#include <libwidget/core/Widget.h>

typedef struct
{
    Widget widget;
} Container;

Widget *container_create(Widget *parent, Rectangle bound);
