#pragma once

#include <libwidget/Widget.h>

struct Panel
{
    Widget widget;
};

Widget *panel_create(Widget *parent);
