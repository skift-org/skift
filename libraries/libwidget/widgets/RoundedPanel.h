#pragma once

#include <libwidget/Widget.h>

struct RoundedPanel
{
    Widget widget;
    int radius;
};

Widget *rounded_panel_create(Widget *parent, int radius);
