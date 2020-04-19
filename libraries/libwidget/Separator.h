#pragma once

#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;
} Separator;

Widget *separator_create(Widget *parent);
