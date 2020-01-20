#pragma once

#include "libwidget/core/Widget.h"

typedef struct
{
    Widget widget;
} Panel;

Widget *panel_create(Widget *parent, Rectangle bound);
