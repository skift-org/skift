#pragma once

#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;
} Panel;

Widget *panel_create(Widget *parent);
