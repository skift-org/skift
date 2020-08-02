#pragma once

#include <libwidget/Widget.h>

struct Separator
{
    Widget widget;
};

Widget *separator_create(Widget *parent);
