#pragma once

#include <libwidget/Widget.h>

struct Container
{
    Widget widget;
};

Widget *container_create(Widget *parent);
