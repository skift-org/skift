#pragma once

#include <libwidget/Widget.h>

struct Placeholder
{
    Widget widget;
    char *text;
};

Widget *placeholder_create(Widget *parent, const char *name);
