#pragma once

#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;
    int radius;
} RoundedPanel;

Widget *rounded_panel_create(Widget *parent, int radius);
